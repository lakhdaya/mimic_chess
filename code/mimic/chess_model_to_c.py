def from_str_to_win(win, color):
    if '1-0' == win:
        return 1
    elif '0-1' == win:
        return 0
    return 0.5  

def from_bitboard_to_fen(board):
    import numpy as np
    list_pieces = ['P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k']
    split_board = np.split(np.array(board), 12)
    fen = []
    index = 0
    for i in range(8):
        index = 0
        for k in range(8):
            piece = False
            for j in range(12):
                if split_board[j][i*8+k]:
                    piece = True
                    if index != 0:
                        fen.append(str(index))
                        index = 0
                    fen.append(list_pieces[j])
                    break
            if not piece:
                index+=1
        if index != 0:
            fen.append(str(index))
        fen.append('/')
    return fen

def extract_moves(games):
    moves = []
    for col_moves in games["moves"]:
        col_moves = col_moves.split(' ')
        moves.append([i if not i[0:1].isdigit() else i.split('.')[1] for i in col_moves][:-1])
    return moves

def load_model(model_path):
    from keras import models
    return models.load_model(model_path)

def convert(l):
    import numpy as np
    return np.int32([int(i) for i in l[1:-1].split(',')])

def load_opening(opening_path):
    import pandas as pd
    opening = pd.read_csv(opening_path)
    opening["bitboard"] = opening["bitboard"].apply(convert)
    return opening

def note_position(model, position):
    import numpy as np
    position =  np.array(position).reshape(1, 773)
    predic = model.predict(position, verbose=0)
    return predic

def note_opening(opening, board):
    import numpy as np
    board = np.int32(board)
    opening = opening[opening["times played"] > 100]
    for row in opening.itertuples(index=False):
        if(np.all(row.bitboard == board)):
            print(row.winrate)
            return row.winrate
    return 2.


    
def extract_game_fen(moves, win, positions, times_played, number_of_win, color):
    board = chess.Board()
    win_int = from_str_to_win(win, color)
    for i, move in enumerate(moves):
        board.push_san(move)
        if((i+color)%2):
            fen = board.epd()
            if fen in positions:
                index = positions.index(fen)
                times_played[index] +=1
                number_of_win[index]+=win_int
            else:
                positions.append(fen)
                number_of_win.append(win_int)
                times_played.append(1)
    return positions, times_played, number_of_win

def add_player_from_our_database(name, path="archive/games_chesstempo.csv"):
    import pandas as pd
    import chess
    file = pd.read_csv(path)
    bplayer_game = file[file["white_player"] == name]
    wplayer_game = file[file["black_player"] == name]
    bmoves = extract_moves(bplayer_game)
    wmoves = extract_moves(wplayer_game)

def test(model):
    print("hello world: ")