from argparse import ArgumentParser
from typing import List
from dataclasses import dataclass
from rich import print

REG_INT = 0
REG_REAL = 1

@dataclass
class Register:
    type: int = REG_INT
    value: int | float = 0

    def cmp_int(self, val: int) -> bool:
        return self.type == REG_INT and self.value == val

    def is_int(self) -> bool:
        return self.type == REG_INT

    def is_real(self) -> bool:
        return self.type == REG_INT


class VM:
    def __init__(self):
        self.registers: dict[str, Register] = {}
        self.source: List[str]
        self.current_line_num: int = 0
        self.current_line: str
        self.is_error: bool = False
        self.got_halt: bool = False
        self.dbg_print_lines: bool = False

    def error(self, msg):
        print(f"[b red]error:[/b red] line {self.current_line_num}: {msg}")
        self.is_error = True

    def check_types_int(self, r1, r2):
        g1 = self.registers[r1]
        g2 = self.registers[r2]
        if not (g1.is_int() and g2.is_int()):
            self.error("both arguments should be ints")

    def check_types_real(self, r1, r2):
        g1 = self.registers[r1]
        g2 = self.registers[r2]
        if not (g1.is_real() and g2.is_real()):
            self.error("both arguments should be reals")


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
            regname, regval = args

            if regval.isnumeric():
                # Direct num
                self.registers[regname] = Register(REG_INT, int(regval))
            else:
                # Issa register
                self.registers[regname] = self.registers[regval]

        elif cmd == "INQL":
            r1, r2, r3 = args
            if self.registers[r2] != self.registers[r3]:
                self.registers[r1] = Register(REG_INT, 1)
            else:
                self.registers[r1] = Register(REG_INT, 0)

        elif cmd == "JMPZ":
            loc, reg = args
            if int(loc) > len(self.source) + 1:
                self.error("line is out of bounds")
                return
            
            if self.registers[reg].cmp_int(0):
                self.current_line_num = int(loc) - 1

        elif cmd == "IPRT":
            print(f"[b]{self.registers[args[0]].value}[/b] [gray] // {args[0]}[/gray]")

        elif cmd == "ISUB":
            r1, r2, r3 = args
            self.check_types_int(r2, r3)
            v1, v2 = map(lambda x: self.registers[x].value, [r2, r3])
            self.registers[r1] = Register(REG_INT, v1 - v2)

        elif cmd == "JUMP":
            loc = int(args[0])
            if loc > len(self.source) + 1:
                self.error("line is out of bounds")
                return
            
            self.current_line_num = loc - 1

        elif cmd == "HALT":
            print("halted")
            self.got_halt = True

        else:
            self.error(f"unknown command {cmd}")




    def run(self):
        if self.is_error:
            print("can't run, error occurred", style="red bold")
            return


def main():
    parser = ArgumentParser()
    parser.add_argument('source_file')
    parser.add_argument('-d')

    args = parser.parse_args()

    vm = VM()
    vm.load_source_file(args.source_file)
    vm.dbg_print_lines = args.d
    vm.parse()

    print("\n[b yellow]POST RUN SUMMARY[/b yellow]")
    print(f"[b]Has Errors?[/b] {vm.is_error}")
    print(f"[b]Line number:[/b] {vm.current_line_num}")
    
    print("[b]Registers:[/b]")
    for k, v in vm.registers.items():
        print(f"{k:4} -> {v}")



if __name__ == "__main__":
    main()