import os
import subprocess
import time
import warnings

import chipwhisperer as cw

from .. import config


def compile(path, cryptooptions=None):
    cryptooptions = cryptooptions or ['CRYPTO_TARGET=NONE']
    try:
        proc = subprocess.run(
            [
                'make',
                '-f',
                f'{os.path.join(os.path.dirname(__file__), "Makefile")}',
                'PLATFORM=CWLITEXMEGA',
                f'FIRMWAREPATH={config.firmwarepath}',
                f'TARGET={os.path.splitext(os.path.basename(path))[0]}',
            ] + cryptooptions,
            cwd=os.path.dirname(path),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=True,
        )
        if config.print_output and config.debug:
            print('\x1b[32m✓\x1b[0m ' + proc.stdout.decode())
    except subprocess.CalledProcessError as e:
        if config.print_output:
            print(f'\x1b[31m✗ "{" ".join(e.args[1])}" returned:\x1b[0m\n' + e.stderr.decode())
        raise


def flash(path):
    prog = cw.programmers.XMEGAProgrammer
    programmed = cw.program_target(
        scope=config.scope,
        prog_type=prog,
        fw_path=os.path.join(
            os.path.dirname(path),
            f'{os.path.splitext(os.path.basename(path))[0]}-CWLITEXMEGA.hex',
        )
    )
    if config.print_output and config.debug:
        print(programmed)


def compile_and_flash(path, cryptooptions=None):
    compile(path, cryptooptions)
    flash(path)
    if config.print_output:
        print('\x1b[32m✓\x1b[0m')


def reset_target():
    scope = config.scope
    target = cw.target(scope)
    scope.io.nrst = 'low'
    time.sleep(0.05)
    target.flush()
    scope.io.nrst = 'high'
    time.sleep(0.05)


def capture():
    scope = config.scope
    target = cw.target(scope)
    ret = scope.capture()

    for i in range(101):
        if target.is_done():
            break
        time.sleep(0.05)
        if i == 100:
            warnings.warn("Target did not finish operation")
            return None
    if ret:
        warnings.warn("Timeout happened during capture")
        return None

    return scope.get_last_trace()
