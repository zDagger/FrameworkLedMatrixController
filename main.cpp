#include "serialib.h"
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <random>
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <unordered_map>
#include <chrono>
#include <thread>

enum directions {
    up,
    down,
    left,
    right
};

//font for winner/looser screens, maps charactor to bitmap
static const std::unordered_map<char, std::array<std::string,7>> FONT5x7 = {
    {'W', {
        "#   #",
        "#   #",
        "#   #",
        "#   #",
        "# # #",
        "## ##",
        "#   #"
    }},
    {'I', {
        "#####",
        "  #  ",
        "  #  ",
        "  #  ",
        "  #  ",
        "  #  ",
        "#####"
    }},
    {'N', {
        "#   #",
        "##  #",
        "# # #",
        "#  ##",
        "#   #",
        "#   #",
        "#   #"
    }},
    {'E', {
        "#####",
        "#    ",
        "#    ",
        "#####",
        "#    ",
        "#    ",
        "#####"
    }},
    {'R', {
        "#### ",
        "#   #",
        "#   #",
        "#### ",
        "# #  ",
        "#  # ",
        "#   #"
    }},
    {'L', {
        "#    ",
        "#    ",
        "#    ",
        "#    ",
        "#    ",
        "#    ",
        "#####"
    }},
    {'O', {
        " ### ",
        "#   #",
        "#   #",
        "#   #",
        "#   #",
        "#   #",
        " ### "
    }},
    {'S', {
        " ####",
        "#    ",
        "#    ",
        " ### ",
        "    #",
        "    #",
        "#### "
    }},
    
};



class randomNum{
    private:
        std::random_device dev;
    public:
        randomNum(){
        }
        //inclusive range
        int genRandom(int lowerBound, int upperBound){
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> dist(lowerBound,upperBound);
            return dist(rng);
        }
};

class serialCommunicator{
    private:
        serialib serial;
    public:
        serialCommunicator(std::string port){
            serial.openDevice(port.c_str(), 115200);
        }
        ~serialCommunicator(){
            serial.closeDevice();
        }
        void sendMessgae(uint8_t commandID, uint8_t message[] = {}, unsigned int size = 0){
            unsigned int serialMessSize = size + 3;
            uint8_t serialMessage[serialMessSize];
            serialMessage[0] = 0x32;
            serialMessage[1] = 0xAC;
            serialMessage[2] = commandID;
            for(uint8_t i = 0; i < size; i++){
                serialMessage[i+3] = message[i];
            }
            serial.writeBytes(serialMessage, serialMessSize);
        }
};

class snakeGame{
    private:
        std::vector<std::vector <int>> snake = {{1,1}};
        std::vector<std::vector<uint8_t>> currBoard;
        std::vector<std::vector<uint8_t>> newBoard;
        std::vector<int> apple = {0,0};
        int width;
        int height;
        int prevDirection = 0;
        bool dead = false;
        bool won = false;
        bool toggle = false;
        randomNum random;
    public:
        snakeGame(int width, int height){
            this->width = width;
            this->height = height;
            this->newApple();
            for(int x=0;x<width;x++){
                currBoard.push_back({});
                for(int y=0;y<height;y++){
                    currBoard[x].push_back(0);
                }
            }
            newBoard = currBoard;
        }
        bool inSnake(std::vector <int> coordinates){
            for(int i = 0; i < snake.size(); i++){
                if(snake[i] == coordinates){
                    return true;
                }
            }
            return false;
        }
        void newApple(){
            this->apple[0] = this->random.genRandom(0,this->width-1);
            this->apple[1] = this->random.genRandom(0,this->height-1);
            while(this->inSnake(apple)){
                this->apple[0] = this->random.genRandom(0,this->width-1);
                this->apple[1] = this->random.genRandom(0,this->height-1);
            }
        }
        void flushBoard(){
            this->currBoard = this->newBoard;
        }
        std::vector<uint8_t> getCurrentConsoleFont(int index){
            return this->currBoard[index];
        }
        std::vector<uint8_t> getNewCol(int index){
            return this->newBoard[index];
        }
        std::vector <int> getApple(){
            return this->apple;
        }
        bool getDead(){
            return this->dead;
        }
        bool getWin(){
            return this->won;
        }
        void fillBoard(uint8_t value){
            for(int x=0;x<this->width;x++){
                for(int y=0;y<this->height;y++){
                    this->newBoard[x][y] = value;
                }
            }
        }
        void drawTextOnBoard(std::string text, uint8_t fg = 255, int scale = 1, int spacing = 1){
           // text = std::string(text.rbegin(),text.rend());
            if (scale < 1) scale = 1;
            int rows = (int)this->newBoard.size();
            if (rows == 0) return;
            int cols = (int)this->newBoard[0].size();

            // convert to all uppercase
            for (auto &ch : text) ch = (char)toupper((unsigned char)ch);

            const int char_w = 5;
            const int char_h = 7;
            int n = (int)text.size();

            // compute required width with given spacing
            auto required_width = [&](int sp) {
                if (n == 0) return 0;
                return n * (char_w * scale) + (n - 1) * sp;
            };

            // try spacing, then fallback to 0 if doesn't fit
            if (required_width(spacing) > cols && spacing > 0) {
                spacing = 0;
            }

            int total_w = required_width(spacing);
            int total_h = char_h * scale;

            // starting positions to center text
            int start_x = std::min(cols, (cols+total_w) / 2);
            //int start_x = cols;
            int start_y = std::max(0, (rows - total_h) / 2);

            int cursor_x = start_x;
            for (char ch : text) {
                auto it = FONT5x7.find(ch);
                if (it == FONT5x7.end()) {
                    // unknown char: advance by blank space of char_w
                    cursor_x -= char_w * scale + spacing;
                    continue;
                }
                const auto &glyph = it->second;
                // glyph: 7 strings (rows), each 5 characters (columns)
                for (int gy = 0; gy < char_h; ++gy) {
                    for (int gx = char_w - 1; gx >= 0; --gx) {
                        if (glyph[gy][gx] == '#') {
                            // plot scaled pixel block
                            for (int sy = 0; sy < scale; ++sy) {
                                for (int sx = 0; sx < scale; ++sx) {
                                    int gx_global = cursor_x - gx * scale - sx;
                                    int gy_global = start_y + gy * scale + sy;
                                    if (gx_global >= 0 && gx_global < cols && gy_global >= 0 && gy_global < rows) {
                                        this->newBoard[gy_global][gx_global] = fg;
                                    }
                                }
                            }
                        }
                    }
                }
                cursor_x -= char_w * scale + spacing;
            }
        }
        void invertGrid(uint8_t value = 255){
            for(int x = 0; x<this->width;x++){
                for(int y=0;y<this->height;y++){
                    if(this->newBoard[x][y] == value){
                        this->newBoard[x][y] = 0;
                    }else if(this->newBoard[x][y] == 0){
                        this->newBoard[x][y] = value;
                    }
                }
            }
        }
        void drawGame(){
            if(this->dead || this->won){
                return;
            }else{
                this->fillBoard(0);
                this->newBoard[this->apple[0]][this->apple[1]] = 255;
                for(int i=0;i<this->snake.size();i++){
                    this->newBoard[this->snake[i][0]][this->snake[i][1]] = 100;
                }
            }
        }
        void move(int direction){
            if(this->dead || this->won){
                return;
            }
            std::vector<int> head = {this->snake.back()[0], this->snake.back()[1]};
            switch(direction){
                case 0:
                    head[1] -= 1;
                case 1:
                    head[1] += 1;
                case 2:
                    head[0] -= 1;
                case 3:
                    head[0] += 1;
            }
            if(std::count(this->snake.begin(),this->snake.end(),head) != 0 || head[0] == this->width || head[0] < 0 || head[1] == this->height || head[1] < 0){
                this->dead = true;
                return;
            }
            this->snake.push_back(head);
            if(head != this->apple){
                this->snake.erase(this->snake.begin());
            }else{
                this->newApple();
            }
        }
};

int main()
{
    std::string ports[2] = {"com5", "com4"};
    serialCommunicator snakeSerial(ports[0]);
    int width = 9;
    int height = 34;
    uint8_t stageCol = 0x07;
    uint8_t flushCols = 0x08;
    uint8_t pattern = 0x01;
    uint8_t example[] = {0x00,30};
    std::chrono::steady_clock::time_point timer;
    bool timerSet = false;
    std::chrono::seconds endTime(10);

    snakeGame Game(width,height);
    while(true){
        if(Game.getDead()||Game.getWin()){
            if(!timerSet){
                timer = std::chrono::steady_clock::now();
                timerSet = true;
                Game.fillBoard(0);
                if(Game.getDead()){
                    Game.drawTextOnBoard("loose");
                }else{
                    Game.drawTextOnBoard("win");
                }
            }else if(std::chrono::steady_clock::now() - timer > endTime){
                break;
            }else{
                std::this_thread::sleep_for(std::chrono::milliseconds(750));
                Game.invertGrid(255);
            }
        }
        Game.drawGame();
        Game.flushBoard();
        for(int x=0;x<width;x++){
            std::vector data = Game.getNewCol(x);
            uint8_t package[height+1];
            package[0] = static_cast <uint8_t> (x);
            for(int i = 1; i<height+1;i++){
                package[i] = data[i-1];
            }
            snakeSerial.sendMessgae(stageCol, package, static_cast <uint8_t> (height + 1));

        }
        snakeSerial.sendMessgae(flushCols);
        Game.move(down);
    }
    //snakeSerial.sendMessgae(pattern, example, 2);

    return 0 ;
}