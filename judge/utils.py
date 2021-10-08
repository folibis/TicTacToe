field = [[]]


def process_message(data) -> str:
    commands = []
    arr = data.split(b",")
    for token in arr:
        pair = token.split(b":")
        if len(pair) == 2:
            commands.append({"name": pair[0].strip(), "value": pair[1].strip()})

    return process_commands(commands)


def get_command_value(name, commands) -> str:
    for com in commands:
        if com["name"] == name:
            return com["value"]
    return ""


def process_commands(commands) -> str:
    command = get_command_value(b"command", commands)
    match command:
        case b"init_game":
            return init_game()
        case b"turn":
            client_id = int(get_command_value(b"id", commands))
            row = int(get_command_value(b"row", commands))
            col = int(get_command_value(b"col", commands))
            return turn(client_id, row, col)

    return ""


def init_game() -> str:
    global field
    field = [[-1 for _ in range(3)] for _ in range(3)]

    return "command: init_game, response: ok"


def turn(client_id, row, col) -> str:
    if client_id < 0 or client_id >= 2:
        return "command: turn, error: wrong id"
    if row < 0 or row >= 3:
        return "command: turn, error: wrong row"
    if col < 0 or col >= 3:
        return "command: turn, error: wrong col"
    if field[row][col] != -1:
        return "command: turn, error: incorrect turn"

    field[row][col] = client_id

    state = check_state()
    if state != -1:
        init_game()
        return "command: turn, id: " + str(client_id) + ", row: " + str(row) + ",col:" + str(col) + ",win:" + str(state)

    state = check_move_available()
    if not state:
        return "command: turn, id: " + str(client_id) + ", row: " + str(row) + ",col:" + str(col) + ",win:-1"

    return "command: turn, id: " + str(client_id) + ", row: " + str(row) + ",col:" + str(col)


def check_state():
    for i in range(3):
        result = check_row(i)
        if result != -1:
            return result
        result = check_col(i)
        if result != -1:
            return result
    result = diagonal1()
    if result != -1:
        return result
    result = diagonal2()
    if result != -1:
        return result

    return -1


def check_move_available():
    for i in range(3):
        for j in range(3):
            if field[i][j] == -1:
                return True
    return False


def check_row(row):
    prev = -1
    for i in range(3):
        if field[row][i] == -1:
            return -1
        if i == 0:
            prev = field[row][i]
        else:
            if field[row][i] != prev:
                return -1
    return prev


def check_col(col):
    prev = -1
    for i in range(3):
        if field[i][col] == -1:
            return -1
        if i == 0:
            prev = field[i][col]
        else:
            if field[i][col] != prev:
                return -1
    return prev


def diagonal1():
    prev = -1
    for i in range(3):
        if field[i][i] == -1:
            return -1
        if i == 0:
            prev = field[i][i]
        else:
            if field[i][i] != prev:
                return -1
    return prev


def diagonal2():
    prev = -1
    for i in range(3):
        if field[i][2 - i] == -1:
            return -1
        if i == 0:
            prev = field[i][2 - i]
        else:
            if field[i][2 - i] != prev:
                return -1
    return prev


