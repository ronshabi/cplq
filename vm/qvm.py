from argparse import ArgumentParser
from typing import List
from rich import print
import sys
import pathlib
import difflib

TInt = 0
TFloat = 1
TRegister = 2

Register = tuple[int, int | float]

def string_is_float(s: str) -> bool:
    try:
        float(s)
        return True
    except ValueError:
        return False


def string_is_int(s: str) -> bool:
    if not s:
        return False
    if s[0] == '+' or s[0] == '-':
        s = s[1:]
    
    return s.isdecimal()


def string_is_register(s: str) -> bool:
    if len(s) < 2:
        return False
    
    if s[0] != 't':
        return False
    
    return string_is_int(s[1:])


def string_get_type(s: str) -> int:
    if string_is_register(s):
        return TRegister
    if string_is_int(s):
        return TInt
    if string_is_float(s):
        return TFloat
    
    raise ValueError(f"Expected {s} to be Int/Float/Register")

def string_is_type(s: str, allowed_types: List[int]):
    t = string_get_type(s)
    return t in allowed_types

def register_equals_int(r: Register, i: int) -> bool:
    return r[0] == TInt and r[1] == i

def register_equals_float(r: Register, f: float) -> bool:
    return r[0] == TFloat and r[1] == f

def register_equals(r1: Register, r2: Register) -> bool:
    return r1[0] == r2[0] and r1[1] == r2[1]

def registers_match_types(r1: Register, r2: Register) -> bool:
    return r1[0] == r2[0]

def registers_match_types_specific(r1: Register, r2: Register, t: int) -> bool:
    return r1[0] == r2[0] and r1[0] == t

def register_from_boolexp(exp: bool) -> Register:
    return Register(TInt, 1 if exp else 0)


class VM:
    def __init__(self):
        self.registers: dict[str, Register] = {}
        self.source: List[str]
        self.current_line_num: int = 0
        self.current_line: str
        self.is_error: bool = False
        self.got_halt: bool = False
        self.dbg_print_lines: bool = False
        self.output: List[str] = []

    def out(self, msg):
        self.output.append(str(msg))
        print(msg)

    def error(self, msg):
        print(f"[b red]error:[/b red] line {self.current_line_num}: {msg}")
        self.is_error = True

    def get_register(self, s: str) -> Register:
        if s not in self.registers:
            raise ValueError(f"Register {s} not found")
        return self.registers[s]

    def ensure_float(self, s: str):
        if string_get_type(s) != TFloat:
            self.error(f"Expected {s} to be a float")

    def ensure_register(self, s: str):
        if string_get_type(s) != TRegister:
            self.error(f"Expected {s} to be a register")

    def ensure_int(self, s: str):
        if string_get_type(s) != TInt:
            self.error(f"Expected {s} to be a float")

    def ensure_int_or_register(self, s: str):
        t = string_get_type(s)
        if t == TFloat:
            self.error('Expected {s} to be an int or a register')
            return False
        
        if t == TInt:
            return True
        
        r = self.get_register(s)
        if r[0] == TFloat:
            self.error('Expected {s} to be an integer register')

    def ensure_float_or_register(self, s: str):
        t = string_get_type(s)
        if t == TInt:
            self.error('Expected {s} to be a float or a register')
            return False
        
        if t == TFloat:
            return True
        
        r = self.get_register(s)
        if r[0] == TInt:
            self.error('Expected {s} to be a float register')

    def ensure_registers_are_of_type(self, r1: Register, r2: Register, t: int):
        if not registers_match_types_specific(r1, r2, t):
            self.error(f"Expected {r1} and {r2} to be of the same type")


    def load_source_file(self, path):
        try:
            with open(path, 'r') as f:
                self.source = f.read().splitlines()
        except Exception:
            self.error("can't open file")

    def parse(self):
        if self.dbg_print_lines:
            print(f"[{self.current_line_num}] ({self.current_line})")

        while self.current_line_num < len(self.source) and not self.is_error and not self.got_halt:
            self.current_line = self.source[self.current_line_num]
            self.current_line_num += 1

            self.parse_line()

        if self.current_line_num == len(self.source):
            if not self.got_halt:
                self.error("didn't get halt at end")

    def parse_line(self):
        cmd, *args = self.current_line.split()

        if cmd == "IASN":
            lhs, rhs = args
            rhs_type = string_get_type(rhs)
            self.ensure_int_or_register(lhs)
            self.ensure_int_or_register(rhs)

            if rhs_type == TRegister:
                self.registers[lhs] = self.get_register(rhs)
            else:
                self.registers[lhs] = Register(TInt, int(rhs))

        elif cmd == "RASN":
            lhs, rhs = args
            rhs_type = string_get_type(rhs)
            self.ensure_float_or_register(lhs)
            self.ensure_float_or_register(rhs)

            if rhs_type == TRegister:
                self.registers[lhs] = self.get_register(rhs)
            else:
                self.registers[lhs] = Register(TFloat, float(rhs))

        elif cmd == "IINP":
            # FIXME: will just input 1 for convenience
            self.registers[args[0]] = Register(TInt, 1)

        elif cmd == "IEQL":
            r1, r2, r3 = args
            self.ensure_register(r1)
            self.ensure_int_or_register(r2)
            self.ensure_int_or_register(r3)
            v1 = self.get_register(r2)
            v2 = self.get_register(r3)
            self.ensure_registers_are_of_type(v1, v2, TInt)
            self.registers[r1] = register_from_boolexp(register_equals(v1, v2))

        elif cmd == "INQL":
            r1, r2, r3 = args
            self.ensure_register(r1)
            self.ensure_int_or_register(r2)
            self.ensure_int_or_register(r3)
            v1 = self.get_register(r2)
            v2 = self.get_register(r3)
            self.ensure_registers_are_of_type(v1, v2, TInt)
            self.registers[r1] = register_from_boolexp(not register_equals(v1, v2))

        elif cmd == "ILSS":
            r1, r2, r3 = args
            self.ensure_register(r1)
            self.ensure_int_or_register(r2)
            self.ensure_int_or_register(r3)
            v1 = self.get_register(r2)
            v2 = self.get_register(r3)
            self.ensure_registers_are_of_type(v1, v2, TInt)
            self.registers[r1] = register_from_boolexp(v1[1] < v2[1])

        elif cmd == "JMPZ":
            loc, reg = args
            if int(loc) > len(self.source) + 1:
                self.error("line is out of bounds")
                return
            
            if self.registers[reg].cmp_int(0):
                self.current_line_num = int(loc) - 1

        elif cmd == "IPRT":
            self.check_type_int(args[0])
            self.out(self.registers[args[0]].value)

        elif cmd == "RPRT":
            self.check_type_real(args[0])
            self.out(self.registers[args[0]].value)


        elif cmd == "RINP":
            # FIXME: will just input 3.14 for convenience
            self.registers[args[0]] = Register(TFloat, 3.14)


        elif cmd == "JUMP":
            loc = int(args[0])
            if loc > len(self.source) + 1:
                self.error("line is out of bounds")
                return
            
            self.current_line_num = loc - 1

        elif cmd == "HALT":
            if self.dbg_print_lines:
                print("halted")

            self.got_halt = True

        else:
            self.error(f"unknown command {cmd}")




    def run(self):
        if self.is_error:
            print("can't run, error occurred", style="red bold")
            return

def check_expected_file(filename: str, source_to_compare: List[str]):
    # replace extension with exp
    exp_filename = filename.replace('.qud', '.exp')
    exp_file = pathlib.Path(exp_filename)
    if not exp_file.exists():
        print(f'[b]exptest:[/b] no exp file for {filename}')
        return False
    
    exp_contents = exp_file.read_text()
    comparison = "\n".join(source_to_compare)

    d = difflib.Differ()
    result = list(d.compare(exp_contents, comparison))
    
    diff_ok = all(map(lambda x: x.startswith(' '), result))

    if not diff_ok:
        print(f'[b red]exptest:[/b red] {filename} failed diff ')
        for line in result:
            if line.startswith('+'):
                print(line, style="green")
            elif line.startswith('-'):
                print(line, style="red")
            else:
                print(line)

        return False
        

    return True
    




def main():
    parser = ArgumentParser()
    parser.add_argument('source_file')
    parser.add_argument('-L', '--print-lines', action='store_true')
    parser.add_argument('-P', '--print-post-run', action='store_true')

    args = parser.parse_args()

    vm = VM()
    vm.load_source_file(args.source_file)
    vm.dbg_print_lines = args.print_lines
    # vm.dbg_print_post_run = args.print_post_run
    vm.parse()

    if args.print_post_run:
        print("\n[b yellow]POST RUN SUMMARY[/b yellow]")
        print(f"[b]Has Errors?[/b] {vm.is_error}")
        print(f"[b]Line number:[/b] {vm.current_line_num}")
        
        print("[b]Registers:[/b]")
        for k, v in vm.registers.items():
            print(f"{k:4} -> {v}")

    if vm.is_error:
        sys.exit(1)

    if not check_expected_file(args.source_file, vm.output):
        sys.exit(1)


if __name__ == "__main__":
    main()