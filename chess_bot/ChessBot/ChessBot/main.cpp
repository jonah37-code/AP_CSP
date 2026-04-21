// Date: 2/9/2026
// Name: Jonah Mayer

#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <cstdint>
#include <array>

struct GameState;

// Utilities
void parseFEN(const std::string& fen, GameState& state);
void printBoard(GameState& state);
char BBToChar(int& BBindex);
int CharToBB(char& pieceChar);


// Move Generation
struct move;
void generateLegalMoves(GameState& state, std::vector<move>& moves);
void generatePseudoLegalMoves(GameState& state, std::vector<move>& moves);
void generatePawnMoves(GameState& state, std::vector<move>& moves);
void generateKnightMoves(GameState& state, std::vector<move>& moves);
void generateBishopMoves(GameState& state, std::vector<move>& moves);
void generateRookMoves(GameState& state, std::vector<move>& moves);
void generateQueenMoves(GameState& state, std::vector<move>& moves);
void generateKingMoves(GameState& state, std::vector<move>& moves);
bool isSquareAttacked(GameState& state, int square, bool byWhite);
bool isInCheck(GameState& state, bool white);

// ******************************************************
// * _   _   _     _   _   _   _   _     _              *
// *| | | | | |_  | | (_) | | (_) | |_  (_)   ___   ___ *
// *| | | | | __| | | | | | | | | | __| | |  / _ \ / __|*
// *| |_| | | |_  | | | | | | | | | |_  | | |  __/ \__ \*
// * \___/   \__| |_| |_| |_| |_|  \__| |_|  \___| |___/*
// ******************************************************

struct GameState {
    std::array <uint64_t, 12> pieces = {};      // WP WN WB WR WQ WK BP BN BB BR BQ BK
    std::array <uint64_t, 3> occupancy = {};    // white, black, both

    uint8_t castlingRights = 0;   // WK WQ BK BQ
    int enPassantSquare = -1; // -1 if none
    bool whiteToMove = true;
    int halfMoveClock = 0;
    int fullMoveClock = 0;
};

void parseFEN(const std::string& fen, GameState& state) {
    std::vector<std::string> fenFields;
    std::stringstream ss(fen);
    std::string s;
    
    while (std::getline(ss, s, ' ')) {
        fenFields.push_back(s);
    }
    
    // Board position
    int rank = 7;
    int file = 0;
    for (char c: fenFields[0]) {
        if (c == '/') {
            file = 0;
            rank -= 1;
        }
        else {
            if (std::isdigit(c)) {
                file += c - '0';
            }
            else {
                state.pieces[CharToBB(c)] |= (1ULL << (rank * 8 + file));
                file += 1;
            }
        }
    }
    
    // Turn
    state.whiteToMove = (fenFields[1] == "w");
    
    // Castling rights
    for (char c: fenFields[2]) {
        if (c == 'K') {
            state.castlingRights |= (1 << 3);
        }
        else if (c == 'Q') {
            state.castlingRights |= (1 << 2);
        }
        else if (c == 'k') {
            state.castlingRights |= (1 << 1);
        }
        else if (c == 'q') {
            state.castlingRights |= (1 << 0);
        }
    }
    
    // En passant square
    if (fenFields[3][0] == '-') {
        state.enPassantSquare = -1;
    }
    else {
        int file = fenFields[3][0] - 'a';
        int rank = fenFields[3][1] - '1';
        state.enPassantSquare = rank * 8 + file;
    }
    
    // Half move clock
    state.halfMoveClock = std::stoi(fenFields[4]);
    
    // Full move clock
    state.fullMoveClock = std::stoi(fenFields[5]);
}

void printBoard(GameState& state) {
    std::array<char, 64> board;
    
    board.fill('.');
    
    for (int b = 0; b < 12; b++) {
        uint64_t boardCopy = state.pieces[b];
        while (boardCopy) {
            board[__builtin_ctzll(boardCopy)] = BBToChar(b);
            boardCopy &= boardCopy - 1;
        }
    }
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            int squareIndex = rank * 8 + file;
            std::cout << board[squareIndex] << " ";
        }
        std::cout << "\n";
    }
}

char BBToChar(int& BBindex) {
    static const std::array<char, 12> BBToChar = {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k'};
    return BBToChar[BBindex];
}

int CharToBB(char& pieceChar) {
    static const std::unordered_map<char, int> CharToBB = {
        {'P', 0}, {'N', 1}, {'B', 2}, {'R', 3}, {'Q', 4}, {'K', 5},
        {'p', 6}, {'n', 7}, {'b', 8}, {'r', 9}, {'q', 10}, {'k', 11}
    };
    return CharToBB.at(pieceChar);
}

// **********************************************************************************************************
// * __  __                             ____                                        _     _                 *
// *|  \/  |   ___   __   __   ___     / ___|   ___   _ __     ___   _ __    __ _  | |_  (_)   ___    _ __  *
// *| |\/| |  / _ \  \ \ / /  / _ \   | |  _   / _ \ | '_ \   / _ \ | '__|  / _` | | __| | |  / _ \  | '_ \ *
// *| |  | | | (_) |  \ V /  |  __/   | |_| | |  __/ | | | | |  __/ | |    | (_| | | |_  | | | (_) | | | | |*
// *|_|  |_|  \___/    \_/    \___|    \____|  \___| |_| |_|  \___| |_|     \__,_|  \__| |_|  \___/  |_| |_|*
// **********************************************************************************************************

struct move {
    int from;
    int to;
    char promotion;
};

void generateLegalMoves(GameState& state, std::vector<move>& moves) {
    generatePseudoLegalMoves(state, moves);
    
}

void generatePseudoLegalMoves(GameState& state, std::vector<move>& moves) {
    generatePawnMoves(state, moves);
    generateKnightMoves(state, moves);
    generateBishopMoves(state, moves);
    generateRookMoves(state, moves);
    generateQueenMoves(state, moves);
    generateKingMoves(state, moves);
}

void generatePawnMoves(GameState& state, std::vector<move>& moves) {
    
    int pawnIndex = state.whiteToMove ? 0 : 6;
    int oppositeColor = state.whiteToMove ? 1 : 0;
    int initialRank = state.whiteToMove ? 1 : 6;
    int promotionRank = state.whiteToMove ? 6 : 1; // rank before promotion
    int direction = state.whiteToMove ? 1 : -1;

    const std::array<char, 4> promotionPieces = {'Q', 'R', 'B', 'N'};

    uint64_t pawns = state.pieces[pawnIndex];

    while (pawns) {
        int from = __builtin_ctzll(pawns);
        pawns &= pawns - 1;

        int rank = from / 8;
        int file = from % 8;

        // Forward move
        int r1 = rank + direction;
        if (r1 >= 0 && r1 < 8) {
            int to1 = r1 * 8 + file;

            if (!(state.occupancy[2] & (1ULL << to1))) {
                if (rank == promotionRank) {
                    for (char p : promotionPieces) {
                        moves.push_back({from, to1, p});
                    }
                }
                else {
                    moves.push_back({from, to1, '\0'});
                }

                // Double push
                if (rank == initialRank) {
                    int r2 = rank + 2 * direction;
                    int to2 = r2 * 8 + file;
                    if (!(state.occupancy[2] & (1ULL << to2))) {
                        moves.push_back({from, to2, '\0'});
                    }
                }
            }
        }

        // Captures
        for (int df : {-1, 1}) {
            int newFile = file + df;
            if (newFile < 0 || newFile > 7) continue;

            int to = r1 * 8 + newFile;
            if ((state.occupancy[oppositeColor] & (1ULL << to)) || (to == state.enPassantSquare)) {
                if (rank == promotionRank) {
                    for (char p : promotionPieces)
                        moves.push_back({from, to, p});
                } else {
                    moves.push_back({from, to, '\0'});
                }
            }
        }
    }
}

void generateKingMoves(GameState& state, std::vector<move>& moves) {
        
    int kingIndex = state.whiteToMove ? 5 : 11;
    int sameColor = state.whiteToMove ? 0 : 1;
    
    const std::array<std::array<int, 2>, 8> moveVectors = {{
        {1,0}, {1,1}, {0,1}, {-1,1},
        {-1,0}, {-1,-1}, {0, -1}, {1, -1}
    }};
    
    int from = __builtin_ctzll(state.pieces[kingIndex]);
    int rank = from / 8;
    int file = from % 8;
    
    for (const auto& moveVector: moveVectors) {
        int r = rank + moveVector[0];
        int f = file + moveVector[1];
        
        if (r >= 0 && r < 8 && f >= 0 && f < 8) {
            int to = r * 8 + f;
            
            if (!(state.occupancy[sameColor] & (1ULL << to))) {
                moves.push_back({from, to, '\0'});
            }
        }
    }
}

void generateKnightMoves(GameState& state, std::vector<move>& moves) {

    int knightIndex = state.whiteToMove ? 1 : 7;
    int sameColor = state.whiteToMove ? 0 : 1;

    uint64_t knights = state.pieces[knightIndex];

    const std::array<std::array<int, 2>, 8> moveVectors = {{
        {2,1}, {1,2}, {-1,2}, {-2,1},
        {-2,-1}, {-1,-2}, {1,-2}, {2,-1}
    }};

    while (knights) {
        int from = __builtin_ctzll(knights);
        knights &= knights - 1;

        int rank = from / 8;
        int file = from % 8;

        for (const auto& jump : moveVectors) {

            int r = rank + jump[0];
            int f = file + jump[1];

            if (r >= 0 && r < 8 && f >= 0 && f < 8) {

                int to = r * 8 + f;

                if (!(state.occupancy[sameColor] & (1ULL << to))) {
                    moves.push_back({from, to, '\0'});
                }
            }
        }
    }
}

void generateBishopMoves(GameState& state, std::vector<move>& moves) {

    int bishopIndex = state.whiteToMove ? 2 : 8;
    int sameColor   = state.whiteToMove ? 0 : 1;

    uint64_t bishops = state.pieces[bishopIndex];

    const std::array<std::array<int,2>,4> moveVectors = {{
        {1,-1},   // NW
        {1, 1},   // NE
        {-1,1},   // SE
        {-1,-1}   // SW
    }};

    while (bishops) {
        int from = __builtin_ctzll(bishops);
        bishops &= bishops - 1;

        int rank = from / 8;
        int file = from % 8;

        for (const auto& vec : moveVectors) {

            int r = rank + vec[0];
            int f = file + vec[1];

            while (r >= 0 && r < 8 && f >= 0 && f < 8) {

                int to = r * 8 + f;

                if (state.occupancy[2] & (1ULL << to)) {

                    if (!(state.occupancy[sameColor] & (1ULL << to)))
                        moves.push_back({from, to, '\0'});

                    break;
                }

                moves.push_back({from, to, '\0'});

                r += vec[0];
                f += vec[1];
            }
        }
    }
}

void generateRookMoves(GameState& state, std::vector<move>& moves) {
    int rookIndex = state.whiteToMove ? 3 : 9; // Determine piece bitboard based on color
    int sameColor = state.whiteToMove ? 0 : 1; // Determine occupancy bitboard for same color pieces

    uint64_t rooks = state.pieces[rookIndex];

    const std::array<std::array<int,2>,4> moveVectors = {{
        {1,0},   // North
        {-1,0},  // South
        {0,1},   // East
        {0,-1}   // West
    }};

    uint64_t allPieces = state.occupancy[2];
    uint64_t friendly  = state.occupancy[sameColor];

    while (rooks) {
        int from = __builtin_ctzll(rooks);
        rooks &= rooks - 1;

        int rank = from / 8;
        int file = from % 8;

        for (const auto& vec : moveVectors) {

            int r = rank + vec[0];
            int f = file + vec[1];

            while (r >= 0 && r < 8 && f >= 0 && f < 8) {

                int to = r * 8 + f;

                if (allPieces & (1ULL << to)) {

                    if (!(friendly & (1ULL << to)))
                        moves.push_back({from, to, '\0'});

                    break;
                }

                moves.push_back({from, to, '\0'});

                r += vec[0];
                f += vec[1];
            }
        }
    }
}

void generateQueenMoves(GameState& state, std::vector<move>& moves) {

    int queenIndex = state.whiteToMove ? 4 : 10;
    int sameColor  = state.whiteToMove ? 0 : 1;

    uint64_t queens = state.pieces[queenIndex];

    const std::array<std::array<int,2>,8> moveVectors = {{
        {1,0}, {-1,0}, {0,1}, {0,-1},   // rook directions
        {1,1}, {1,-1}, {-1,1}, {-1,-1}  // bishop directions
    }};

    uint64_t allPieces = state.occupancy[2];
    uint64_t friendly  = state.occupancy[sameColor];

    while (queens) {
        int from = __builtin_ctzll(queens);
        queens &= queens - 1;

        int rank = from / 8;
        int file = from % 8;

        for (const auto& vec : moveVectors) {

            int r = rank + vec[0];
            int f = file + vec[1];

            while (r >= 0 && r < 8 && f >= 0 && f < 8) {

                int to = r * 8 + f;

                if (allPieces & (1ULL << to)) {

                    if (!(friendly & (1ULL << to)))
                        moves.push_back({from, to, '\0'});

                    break;
                }

                moves.push_back({from, to, '\0'});

                r += vec[0];
                f += vec[1];
            }
        }
    }
}

bool isSquareAttacked(GameState& state, int square, bool byWhite) {

    uint64_t bit = 1ULL << square;

    int pawn   = byWhite ? 0 : 6;
    int knight = byWhite ? 1 : 7;
    int bishop = byWhite ? 2 : 8;
    int rook   = byWhite ? 3 : 9;
    int queen  = byWhite ? 4 : 10;
    int king   = byWhite ? 5 : 11;

    int rank = square >> 3;
    int file = square & 7;

    // pawn attacks
    if (byWhite) {
        if (file > 0) {
            int s = square - 9;
            if (s >= 0 && (state.pieces[pawn] & (1ULL << s))) return true;
        }
        if (file < 7) {
            int s = square - 7;
            if (s >= 0 && (state.pieces[pawn] & (1ULL << s))) return true;
        }
    } else {
        if (file > 0) {
            int s = square + 7;
            if (s < 64 && (state.pieces[pawn] & (1ULL << s))) return true;
        }
        if (file < 7) {
            int s = square + 9;
            if (s < 64 && (state.pieces[pawn] & (1ULL << s))) return true;
        }
    }

    // knight attacks
    const int knightOffsets[8] = {17,15,10,6,-6,-10,-15,-17};

    for (int offset : knightOffsets) {
        int s = square + offset;
        if (s < 0 || s >= 64) continue;

        int r = s >> 3;
        int f = s & 7;

        if (abs(r - rank) > 2 || abs(f - file) > 2) continue;

        if (state.pieces[knight] & (1ULL << s)) return true;
    }

    // king attacks
    const int kingOffsets[8] = {1,-1,8,-8,9,-9,7,-7};

    for (int offset : kingOffsets) {
        int s = square + offset;
        if (s < 0 || s >= 64) continue;

        int r = s >> 3;
        int f = s & 7;

        if (abs(r - rank) > 1 || abs(f - file) > 1) continue;

        if (state.pieces[king] & (1ULL << s)) return true;
    }

    // rook/queen attacks
    const int rookDirs[4] = {8,-8,1,-1};

    for (int dir : rookDirs) {

        int s = square;

        while (true) {
            int prev = s;
            s += dir;

            if (s < 0 || s >= 64) break;

            int r = s >> 3;
            int f = s & 7;

            if (dir == 1 || dir == -1) {
                if (abs(f - (prev & 7)) != 1) break;
            }

            uint64_t sq = 1ULL << s;

            if (state.occupancy[2] & sq) {
                if ((state.pieces[rook] & sq) || (state.pieces[queen] & sq))
                    return true;
                break;
            }
        }
    }

    // bishop/queen attacks
    const int bishopDirs[4] = {9,7,-7,-9};

    for (int dir : bishopDirs) {

        int s = square;

        while (true) {
            int prev = s;
            s += dir;

            if (s < 0 || s >= 64) break;

            int r = s >> 3;
            int f = s & 7;

            if (abs((prev & 7) - f) != 1) break;

            uint64_t sq = 1ULL << s;

            if (state.occupancy[2] & sq) {
                if ((state.pieces[bishop] & sq) || (state.pieces[queen] & sq))
                    return true;
                break;
            }
        }
    }

    return false;
}

bool isInCheck(GameState& state, bool white) {
    int kingSquare = __builtin_ctzll(state.pieces[white ? 5 : 11]);
    return isSquareAttacked(state, kingSquare, !white);
}

// ***************************************************************************************************************************
// * __  __           _                                                                                                      *
// *|  \/  |   __ _  (_)  _ __                                                                                               *
// *| |\/| |  / _` | | | | '_ \                                                                                              *
// *| |  | | | (_| | | | | | | |                                                                                             *
// *|_|  |_|  \__,_| |_| |_| |_|                                                                                             *
// ***************************************************************************************************************************

int main() {
    GameState state;
    parseFEN("8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - - 99 50", state);
    printBoard(state);
    std::cout << state.pieces[2];
}
