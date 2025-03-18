from typing import List
from argparse import ArgumentParser
from rich import print as rprint
import sys

TInt = 0
TFloat = 1
TIntRegister = 2
TFloatRegister = 3


def type_to_string(t):
    if t == TInt:
        return "TInt"
    if t == TFloat:
        return "TFloat"
    if t == TIntRegister:
        return "TIntRegister"
    if t == TFloatRegister:
        return "TFloatRegister"

    raise ValueError("Unknown type")


class VMException(Exception):
    pass


def register_type_check(method):
    def wrapper(self, other):
        if isinstance(other, Register):
            if self.T != other.T:
                raise ValueError("Types of registers do not match")
            return method(self, other)
        return NotImplemented

    return wrapper


class Register:
    def __init__(self, tp, val):
        self._tp = tp
        self._set_val(val)

    def _set_val(self, val):
        self._val = val

        if isinstance(val, str):
            if self._tp == TInt:
                try:
                    self._val = int(val)
                except ValueError:
                    raise VMException("Expected an integer-string for register")
            elif self._tp == TFloat:
                try:
                    self._val = float(val)
                except ValueError:
                    raise VMException("Expected a float-string for register")
        elif isinstance(val, float):
            if self._tp != TFloat:
                raise VMException("Expected a float for register")
        elif isinstance(val, int):
            # raise VMException(f"Expected an integer for register, got {val} instead")
            pass
        elif isinstance(val, bool):
            if self._tp != TInt:
                raise VMException("Expected an integer for register from boolean")
            self._val = 1 if val else 0
        else:
            raise VMException(f"Type of register value {val} is unknown")

    @property
    def val(self):
        return self._val

    @property
    def T(self):
        return self._tp

    @val.setter
    def val(self, new_val):
        self._set_val(new_val)

    @register_type_check
    def __lt__(self, other):
        return self.val < other.val

    @register_type_check
    def __le__(self, other):
        return self.val <= other.val

    @register_type_check
    def __eq__(self, other):
        return self.val == other.val

    @register_type_check
    def __ne__(self, other):
        return self.val != other.val

    @register_type_check
    def __gt__(self, other):
        return self.val > other.val

    @register_type_check
    def __ge__(self, other):
        return self.val >= other.val

    def __repr__(self):
        if self._tp == TInt:
            typestring = "TInt"
        else:
            typestring = "TFloat"

        return f"Register({typestring}, {self._val})"


class VM:
    def __init__(self):
        self._filename: str = ""
        self._source: List[str] = []
        self._line_no: int = 0
        self._has_errors: bool = False
        self._registers: dict[str, Register] = {}
        self._is_running = True

    def error(self, message):
        rprint(
            f"[red]VMError[/red]: [b white]{self._filename}:{self._line_no}[/b white]: {message}"
        )
        self._has_errors = True

    def set_source_file(self, srcfile: str):
        self._filename = srcfile
        with open(srcfile, "r") as f:
            self._source = f.read().splitlines()

    def run(self):
        while (
            self._line_no < len(self._source)
            and not self._has_errors
            and self._is_running
        ):
            line = self._source[self._line_no]
            self._line_no += 1
            op, *args = line.split()

            # Find the function implementing this opcode
            func = getattr(self, op)

            # Check that the argument count for this function
            # is correct
            func_arg_count = func.__code__.co_argcount

            # print("funcname:", func.__code__.co_name)

            if func_arg_count - 1 != len(args):
                self.error(
                    f"Opcode {op} requires {func_arg_count - 1} arguments, {len(args)} given"
                )

            try:
                func(*args)
            except VMException as vme:
                self.error(vme)
            except KeyError as ke:
                self.error(f"register not found: {ke}")

    def require_param(self, param: str, ptype):
        recognized = []

        if param.startswith("t") and param[1:].isdigit():
            # Try and search that register
            if param in self._registers:
                reg = self._registers[param]
                if reg.T == TInt:
                    recognized.append(TIntRegister)
                else:
                    recognized.append(TFloatRegister)
            else:
                # Append both, we allow it because the reg isn't known
                recognized.append(TIntRegister)
                recognized.append(TFloatRegister)
        else:
            try:
                int(param)
                recognized.append(TInt)
                recognized.append(TFloat)
            except ValueError:
                try:
                    float(param)
                    recognized.append(TFloat)
                except ValueError:
                    pass

        # Now check if we recognized it
        if not set(recognized).intersection(set(ptype)):
            recognized_strings = [type_to_string(x) for x in recognized]
            ptype_strings = [type_to_string(x) for x in ptype]

            raise VMException(
                f"Expected types {ptype_strings}, got {recognized_strings} instead"
            )

    def as_register(self, tp: int, exp: str) -> Register:
        # If it's a register name, get its value
        if exp.startswith("t"):
            return Register(tp, self._registers[exp].val)
        return Register(tp, exp)

    def get_register_or_imm_value(self, exp: str) -> str:
        if exp.startswith("t"):
            return str(self._registers[exp].val)
        return exp

    # --- I-Funcs --------------------------------------------------------------

    def IASN(self, a, b):
        self.require_param(a, [TIntRegister])
        self.require_param(b, [TIntRegister, TInt])
        self._registers[a] = self.as_register(TInt, b)

    def IPRT(self, b):
        self.require_param(b, [TIntRegister, TInt])
        print(self.get_register_or_imm_value(b))

    def IINP(self, a):
        self.require_param(a, [TIntRegister])
        self._registers[a] = Register(TInt, 1)

    def IEQL(self, a, b, c):
        self.require_param(a, [TIntRegister])
        self.require_param(b, [TIntRegister, TInt])
        self.require_param(c, [TIntRegister, TInt])
        br = self.as_register(TInt, b)
        cr = self.as_register(TInt, c)
        self._registers[a] = Register(TInt, br == cr)

    def INQL(self, a, b, c):
        self.require_param(a, [TIntRegister])
        self.require_param(b, [TIntRegister, TInt])
        self.require_param(c, [TIntRegister, TInt])
        br = self.as_register(TInt, b)
        cr = self.as_register(TInt, c)
        self._registers[a] = Register(TInt, br != cr)

    def ILSS(self, a, b, c):
        self.require_param(a, [TIntRegister])
        self.require_param(b, [TIntRegister, TInt])
        self.require_param(c, [TIntRegister, TInt])
        br = self.as_register(TInt, b)
        cr = self.as_register(TInt, c)
        self._registers[a] = Register(TInt, br < cr)

    def IGRT(self, a, b, c):
        self.require_param(a, [TIntRegister])
        self.require_param(b, [TIntRegister, TInt])
        self.require_param(c, [TIntRegister, TInt])
        br = self.as_register(TInt, b)
        cr = self.as_register(TInt, c)
        self._registers[a] = Register(TInt, br > cr)

    def IADD(self, a, b, c):
        self.require_param(a, [TIntRegister])
        self.require_param(b, [TIntRegister, TInt])
        self.require_param(c, [TIntRegister, TInt])
        br = self.as_register(TInt, b)
        cr = self.as_register(TInt, c)
        self._registers[a] = Register(TInt, br.val + cr.val)

    def ISUB(self, a, b, c):
        self.require_param(a, [TIntRegister])
        self.require_param(b, [TIntRegister, TInt])
        self.require_param(c, [TIntRegister, TInt])
        br = self.as_register(TInt, b)
        cr = self.as_register(TInt, c)
        self._registers[a] = Register(TInt, br.val - cr.val)

    def IMLT(self, a, b, c):
        self.require_param(a, [TIntRegister])
        self.require_param(b, [TIntRegister, TInt])
        self.require_param(c, [TIntRegister, TInt])
        br = self.as_register(TInt, b)
        cr = self.as_register(TInt, c)
        self._registers[a] = Register(TInt, br.val * cr.val)

    def IDIV(self, a, b, c):
        self.require_param(a, [TIntRegister])
        self.require_param(b, [TIntRegister, TInt])
        self.require_param(c, [TIntRegister, TInt])
        br = self.as_register(TInt, b)
        cr = self.as_register(TInt, c)
        self._registers[a] = Register(TInt, br.val // cr.val)

    def ITOR(self, d, b):
        self.require_param(d, [TFloatRegister])
        self.require_param(b, [TIntRegister, TInt])
        self._registers[d] = self.as_register(TFloat, b)

    # --- R-Funcs --------------------------------------------------------------

    def RASN(self, a, b):
        self.require_param(a, [TFloatRegister])
        self.require_param(b, [TFloatRegister, TFloat])
        self._registers[a] = self.as_register(TFloat, b)

    def RPRT(self, b):
        self.require_param(b, [TFloatRegister, TFloat])
        print(self.get_register_or_imm_value(b))

    def RINP(self, a):
        self.require_param(a, [TFloatRegister])
        self._registers[a] = Register(TFloat, 1)

    def REQL(self, a, b, c):
        self.require_param(a, [TFloatRegister])
        self.require_param(b, [TFloatRegister, TFloat])
        self.require_param(c, [TFloatRegister, TFloat])
        br = self.as_register(TFloat, b)
        cr = self.as_register(TFloat, c)
        self._registers[a] = Register(TInt, br == cr)

    def RNQL(self, a, b, c):
        self.require_param(a, [TFloatRegister])
        self.require_param(b, [TFloatRegister, TFloat])
        self.require_param(c, [TFloatRegister, TFloat])
        br = self.as_register(TFloat, b)
        cr = self.as_register(TFloat, c)
        self._registers[a] = Register(TInt, br != cr)

    def RLSS(self, a, b, c):
        self.require_param(a, [TFloatRegister])
        self.require_param(b, [TFloatRegister, TFloat])
        self.require_param(c, [TFloatRegister, TFloat])
        br = self.as_register(TFloat, b)
        cr = self.as_register(TFloat, c)
        self._registers[a] = Register(TInt, br < cr)

    def RGRT(self, a, b, c):
        self.require_param(a, [TFloatRegister])
        self.require_param(b, [TFloatRegister, TFloat])
        self.require_param(c, [TFloatRegister, TFloat])
        br = self.as_register(TFloat, b)
        cr = self.as_register(TFloat, c)
        self._registers[a] = Register(TInt, br > cr)

    def RADD(self, a, b, c):
        self.require_param(a, [TFloatRegister])
        self.require_param(b, [TFloatRegister, TFloat])
        self.require_param(c, [TFloatRegister, TFloat])
        br = self.as_register(TFloat, b)
        cr = self.as_register(TFloat, c)
        self._registers[a] = Register(TFloat, br.val + cr.val)

    def RSUB(self, a, b, c):
        self.require_param(a, [TFloatRegister])
        self.require_param(b, [TFloatRegister, TFloat])
        self.require_param(c, [TFloatRegister, TFloat])
        br = self.as_register(TFloat, b)
        cr = self.as_register(TFloat, c)
        self._registers[a] = Register(TFloat, br.val - cr.val)

    def RMLT(self, a, b, c):
        self.require_param(a, [TFloatRegister])
        self.require_param(b, [TFloatRegister, TFloat])
        self.require_param(c, [TFloatRegister, TFloat])
        br = self.as_register(TFloat, b)
        cr = self.as_register(TFloat, c)
        self._registers[a] = Register(TFloat, br.val * cr.val)

    def RDIV(self, a, b, c):
        self.require_param(a, [TFloatRegister])
        self.require_param(b, [TFloatRegister, TFloat])
        self.require_param(c, [TFloatRegister, TFloat])
        br = self.as_register(TFloat, b)
        cr = self.as_register(TFloat, c)
        self._registers[a] = Register(TFloat, br.val // cr.val)

    def RTOI(self, a, e):
        self.require_param(a, [TIntRegister])
        self.require_param(e, [TFloatRegister, TFloat])
        self._registers[a] = self.as_register(TInt, e)

    # --- Misc -----------------------------------------------------------------
    def HALT(self):
        self._is_running = False

    def JMPZ(self, ln, a):
        self.require_param(ln, [TInt])
        self.require_param(a, [TIntRegister])
        if int(ln) > len(self._source) + 1:
            raise VMException(f"Jump-target {ln} is out of bounds")

        a_val = self._registers[a].val

        if a_val == 0:
            self._line_no = int(ln) - 1

    def JUMP(self, ln):
        self.require_param(ln, [TInt])
        if int(ln) > len(self._source) + 1:
            raise VMException(f"Jump-target {ln} is out of bounds")

        self._line_no = int(ln) - 1


def main():
    parser = ArgumentParser()
    parser.add_argument("source_file")
    parser.add_argument("-L", "--print-lines", action="store_true")
    parser.add_argument("-P", "--print-post-run", action="store_true")

    args = parser.parse_args()

    vm = VM()
    vm.set_source_file(args.source_file)
    # vm.dbg_print_post_run = args.print_post_run
    vm.run()

    # if args.print_post_run:
    #     print("\n[b yellow]POST RUN SUMMARY[/b yellow]")
    #     print(f"[b]Has Errors?[/b] {vm.is_error}")
    #     print(f"[b]Line number:[/b] {vm.current_line_num}")

    #     print("[b]Registers:[/b]")
    #     for k, v in vm._registers.items():
    #         print(f"{k:4} -> {v}")

    if vm._has_errors:
        sys.exit(1)


if __name__ == "__main__":
    main()
