#!/usr/bin/env python
# coding: utf-8

def from_fen_to_bitboard(fen):
    position_pieces = {'P':0, 'N':1, 'B':2, 'R':3, 'Q':4, 'K':5, 'p':6, 'n':7, 'b':8, 'r':9, 'q':10, 'k':11}
    list_pieces = ['P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k']
    castle_dico = {'K':1, 'Q':0, 'k':2, 'q':3}
    position, turn, castle, ep = fen.split(' ')
    board = [0]*12*64
    index = 0
    for p in position[::-1]:
        if p in position_pieces:
            board[index+position_pieces[p]*64] = 1
            index+=1
        elif p != "/":
            index+=int(p)
    turn = 0 if turn=='w' else 0
    castle_bit=[0, 0, 0, 0]
    if(castle != '-'):
        for c in castle:
            castle_bit[castle_dico[c]] = 1
    return board+[turn]+castle_bit

def from_bitboard_to_fen(board):
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
    from tqdm import tqdm
    moves = []
    for col_moves in games["moves"]:
        col_moves = col_moves.split(' ')
        moves.append([i if not i[0:1].isdigit() else i.split('.')[1] for i in col_moves][:-1])
    for i, move in tqdm(enumerate(moves)):
        for j, m in enumerate(move):
            if "e.p" in m:
                moves[i].pop(j)
    return moves


# In[66]:


def from_str_to_int_win(win):
    if '1-0' == win:
        return 1
    elif '0-1' == win:
        return 0
    return 0.5


# In[67]:


def extract_game_fen_color(moves, win, positions, times_played, number_of_win, color):
    import chess
    board = chess.Board()
    win_int = from_str_to_int_win(win)
    for i, move in enumerate(moves):
        board.push_san(move)
        if(((i+color) % 2) == 0):
            fen = board.epd()
            if fen in positions:
                index = positions.index(fen)
                times_played[index]+=1
                number_of_win[index]+=win_int
            else:
                positions.append(fen)
                number_of_win.append(win_int)
                times_played.append(1)
    return positions, times_played, number_of_win


def extract_game_fen(moves, win, positions, times_played, number_of_win):
    import chess
    board = chess.Board()
    win_int = from_str_to_int_win(win)
    for move in moves:
        board.push_san(move)
        fen = board.epd()
        if fen in positions:
            index = positions.index(fen)
            times_played[index]+=1
            number_of_win[index]+=win_int
        else:
            positions.append(fen)
            number_of_win.append(win_int)
            times_played.append(1)
    return positions, times_played, number_of_win




def extract_positions_color(games, moves, color):
    from tqdm import tqdm
    positions = []
    times_played = []
    number_of_win = []
    for win, move in tqdm(zip(games["result"], moves), total=len(moves)):
        positions, times_played, number_of_win = extract_game_fen_color(move, win, positions, times_played, number_of_win, color)
    return positions, times_played, number_of_win

def extract_opening_color(games, moves, color):
    from tqdm import tqdm
    positions = []
    times_played = []
    number_of_win = []
    for win, move in tqdm(zip(games["result"], moves), total=len(moves)):
        positions, times_played, number_of_win = extract_game_fen_color(move[:10], win, positions, times_played, number_of_win, color)
    return positions, times_played, number_of_win

def extract_positions(games, moves):
    from tqdm import tqdm
    positions = []
    times_played = []
    number_of_win = []
    for win, move in tqdm(zip(games["result"], moves), total=len(moves)):
        positions, times_played, number_of_win = extract_game_fen(move, win, positions, times_played, number_of_win)
    return positions, times_played, number_of_win

def extract_opening(games, moves):
    from tqdm import tqdm
    positions = []
    times_played = []
    number_of_win = []
    for win, move in tqdm(zip(games["result"], moves), total=len(moves)):
        positions, times_played, number_of_win = extract_game_fen(move[:10], win, positions, times_played, number_of_win)
    return positions, times_played, number_of_win

# In[72]:

def get_player_name():
    import pandas as pd
    games = pd.read_csv("mimic/archive/chess/players.csv")
    for name in games["name"]:
        print(name)

def get_data_player():
    import os
    print(os.listdir("mimic/data"))

def extract_games():
    import pandas as pd
    import numpy as np
    from tqdm import tqdm 
    games = pd.read_csv("mimic/archive/chess/games_chesstempo.csv")
    player_file = "complete"
    games = games
    moves = extract_moves(games)[:10000]
    positions, times_played, number_of_win = extract_positions(games, moves)
    fen_games = pd.DataFrame.from_dict({"positions": positions, "times played":times_played, "number of win":number_of_win})
    fen_games["bitboard"] = fen_games["positions"].apply(from_fen_to_bitboard)
    fen_games["winrate"] = fen_games["number of win"] / fen_games["times played"]
    fen_games.to_csv("mimic/data/"+player_file+".csv", index=None)

def extract_games_opening():
    import pandas as pd
    import numpy as np
    from tqdm import tqdm 
    games = pd.read_csv("mimic/archive/chess/games_chesstempo.csv")
    player_file = "complete_opening"
    moves = extract_moves(games)
    positions, times_played, number_of_win = extract_opening(games, moves)
    fen_games = pd.DataFrame.from_dict({"positions": positions, "times played":times_played, "number of win":number_of_win})
    fen_games["bitboard"] = fen_games["positions"].apply(from_fen_to_bitboard)
    fen_games["winrate"] = fen_games["number of win"] / fen_games["times played"]
    fen_games.to_csv("mimic/data/"+player_file+".csv", index=None)

def extract_games_player(player):
    import pandas as pd
    import numpy as np
    from tqdm import tqdm 
    print(player) 
    games = pd.read_csv("mimic/archive/chess/games_chesstempo.csv")
    player_file = player.replace(', ', '_').strip(',').lower()
    wgames = games[games["white_player"] == player]
    wmoves = extract_moves(wgames)
    wpositions, wtimes_played, wnumber_of_win = extract_positions_color(wgames, wmoves, 1)
    bgames = games[games["black_player"] == player]
    bmoves = extract_moves(bgames)
    if(len(bmoves) == 0 and len(wmoves) == 0):
        print("joueur non trouve")
        return
    bpositions, btimes_played, bnumber_of_win = extract_positions_color(bgames, bmoves, 0)
    positions = wpositions + bpositions
    times_played = wtimes_played + btimes_played
    number_of_win = wnumber_of_win + bnumber_of_win
    fen_games = pd.DataFrame.from_dict({"positions": positions, "times played":times_played, "number of win":number_of_win})
    fen_games["bitboard"] = fen_games["positions"].apply(from_fen_to_bitboard)
    fen_games["winrate"] = fen_games["number of win"] / fen_games["times played"]
    fen_games.to_csv("mimic/data/"+player_file+".csv", index=None)

def extract_games_opening_player(player):
    import pandas as pd
    import numpy as np
    from tqdm import tqdm 
    print(player) 
    games = pd.read_csv("mimic/archive/chess/games_chesstempo.csv")
    player_file = player.replace(', ', '_').strip(',').lower()
    wgames = games[games["white_player"] == player]
    wmoves = extract_moves(wgames)
    wpositions, wtimes_played, wnumber_of_win = extract_opening_color(wgames, wmoves, 1)
    bgames = games[games["black_player"] == player]
    bmoves = extract_moves(bgames)
    if(len(bmoves) == 0 and len(wmoves) == 0):
        print("joueur non trouve")
        return
    bpositions, btimes_played, bnumber_of_win = extract_opening_color(bgames, bmoves, 0)
    positions = wpositions + bpositions
    times_played = wtimes_played + btimes_played
    number_of_win = wnumber_of_win + bnumber_of_win
    fen_games = pd.DataFrame.from_dict({"positions": positions, "times played":times_played, "number of win":number_of_win})
    fen_games["bitboard"] = fen_games["positions"].apply(from_fen_to_bitboard)
    fen_games["winrate"] = fen_games["number of win"] / fen_games["times played"]
    fen_games.to_csv("mimic/data/"+player_file+"_opening.csv", index=None)

