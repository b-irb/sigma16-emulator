import sigma16
import curses

RRR_MNEMONICS = [
    "add",   "sub", "mul", "div", "cmp", "cmplt", "cmpeq",
    "cmpgt", "inv", "and", "or",  "xor", "nop",   "trap"
]

RX_MNEMONICS = [
    "lea", "load", "store", "jump", "jumpc0", "jumpc1",
    "jumpf", "jumpt", "jal"
]


class Tracer:
    def __init__(self, stdscr):
        self.stdscr = stdscr
        curses.start_color()

        curses.init_pair(1, curses.COLOR_RED, curses.COLOR_BLACK)
        curses.init_pair(2, curses.COLOR_GREEN, curses.COLOR_BLACK)
        curses.init_pair(3, curses.COLOR_BLUE, curses.COLOR_BLACK)
        curses.init_pair(4, curses.COLOR_YELLOW, curses.COLOR_BLACK)

        self.execution_win = curses.newwin(40, 30, 0, 0)
        self.cpu_state = curses.newwin(80, 30, 0, 30)

        self._execution_win_idx = -1

        stdscr.clear()

    def _append_exec_win(self, string, colour=0):
        self._execution_win_idx = (self._execution_win_idx + 1) % 39
        if self._execution_win_idx == 1:
            self.execution_win.clear()

        self.execution_win.addstr(self._execution_win_idx, 0, string, colour)

    def __call__(self, instruction):
        def rrr_handler(instruction):
            TEMPLATE = "{m}\tR{i.d}, R{i.sa}, R{i.sb}"
            self._append_exec_win(TEMPLATE.format(m=RRR_MNEMONICS[instruction.op], i=instruction), curses.color_pair(1))

        def rx_handler(instruction):
            if instruction.sb > 2:
                colour = curses.color_pair(4)
            else:
                colour = curses.color_pair(2)

            TEMPLATE = "{m}\tR{i.d}, {i.disp}[R{i.sa}]"
            self._append_exec_win(TEMPLATE.format(m=RX_MNEMONICS[instruction.sb], i=instruction), colour)

        handlers = {
            sigma16.InstructionRRR: rrr_handler,
            sigma16.InstructionRX: rx_handler,
        }
        handlers[type(instruction)](instruction)
        self.execution_win.refresh()
        self.stdscr.refresh()
        self.stdscr.getkey()


def main(stdscr):
    tracer = Tracer(stdscr)
    emu = sigma16.Emulator("stackarray_simple.bin", trace_handler=tracer)
    emu.execute()

if __name__ == "__main__":
    curses.wrapper(main)
