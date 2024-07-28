import subprocess

board_file = 'board.csv'
jump_executable = 'cmake-build-debug/Jump'


if __name__ == '__main__':
    command = [jump_executable, board_file]
    try:
        completed_process = subprocess.run(command)
    except subprocess.SubprocessError as e:
        print(e)
