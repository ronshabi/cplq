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

    

class VM:
    def __init__(self):
        self.registers: dict[str, Register] = {}
        self.source: List[str]
        self.current_line_num: int = 0
        self.current_line: str
        self.is_error: bool = False

    def error(self, msg):
        print(f"[b red]error:[/b red] line {self.current_line_num}: {msg}")
        self.is_error = True


    def load_source_file(self, path):
        try:
            with open(path, 'r') as f:
                self.source = f.read().splitlines()
        except Exception:
            self.error("can't open file")

    def parse(self):
        while self.current_line_num < len(self.source) and not self.is_error:
            self.current_line = self.source[self.current_line_num]
            self.current_line_num += 1

            self.parse_line()


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

        else:
            self.error(f"unknown command {cmd}")


        print(f"parsing line {self.current_line_num}: ({self.current_line}) -> {cmd}")


    def run(self):
        if self.is_error:
            print("can't run, error occurred", style="red bold")
            return


def main():
    parser = ArgumentParser()
    parser.add_argument('source_file')

    args = parser.parse_args()

    vm = VM()
    vm.load_source_file(args.source_file)
    vm.parse()

    print("\n[b yellow]POST RUN SUMMARY[/b yellow]")
    print(f"[b]Has Errors?[/b] {vm.is_error}")
    print(f"[b]Line number:[/b] {vm.current_line_num}")
    
    print("[b]Registers:[/b]")
    for k, v in vm.registers.items():
        print(f"{k:4} -> {v}")



if __name__ == "__main__":
    main()