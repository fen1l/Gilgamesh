#include <cstdint>
#include <linux/input-event-codes.h>
#include <string>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include "iostream"
#include <sys/ioctl.h>
#include <vector>
#include <fcntl.h>
#include <linux/input.h>
#include <chrono>
#include <termios.h>

using namespace std;
int main() {
    //disabling echo
    termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ECHO;
    t.c_cc[VMIN] = 1;
    t.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    winsize ws{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    const int height=ws.ws_row;
    const int width=ws.ws_col;
    const int depth=1000;
    const int charperframe=height*width;
    const char* keyboard="/dev/input/event9";
    const char* mouse="/dev/input/event5";
    cout<<"1";
    
    //intializing map vector/space    
    
    std::vector<uint8_t> map;
    map.reserve(height*width*depth);

    //initializing initial player position to middle most point

    int ph=height/2;
    int pw=width/2;
    int pd=depth/2;

    //getting in keyboard input
    //w=17 a=20 s=31 d=32
    int keyinputopen=open(keyboard, O_RDONLY | O_NONBLOCK);
    int mouseinputopen=open(mouse, O_RDONLY | O_NONBLOCK);
    if(keyinputopen==-1 || mouseinputopen==-1){
        perror("i won't");
        return 0;
    }
    input_event keybinputs;
    input_event mouseinputs;
    int keyth=1;//a threshold i set for the keystroke (wasd,space) to actually be registered by the engine as they were being registered too fast to the point where one stroke were registered at +2 on all three axis but it did not work we will see to this when working on rendering, i am sure we will have to limit the player speed this way
    int wcounter=0;
    int acounter=0;
    int scounter=0;
    int dcounter=0;
    int spacecounter=0;
    while(true){
        while (read(keyinputopen,&keybinputs,sizeof(keybinputs))==sizeof(keybinputs)){
            // cout<<keybinputs.code<<"\n";
            if (ph!=height){
                if(spacecounter==keyth){
                    ph+=(keybinputs.code==57);
                    spacecounter=0;
                }
                else{
                    spacecounter+=1;
                }
            }
            if (pw!=width){
                if(dcounter==keyth){
                    pw+=(keybinputs.code==32);
                    dcounter=0;
                }
                else{
                    dcounter+=1;
                }
            }
            if (pw!=0){
                if(acounter==keyth){
                    pw-=(keybinputs.code==30);
                    acounter=0;
                }
                else{
                    acounter+=1;
                }
            }
            if (pd!=depth){
                if(wcounter==keyth){
                    pd+=(keybinputs.code==17);
                    wcounter=0;
                }
                else{
                    wcounter+=1;
                }
            }
            if (pd!=0){
                if(scounter==keyth){
                    pd-=(keybinputs.code==31);
                    scounter=0;
                }
                else{
                    scounter+=1;
                }

            }
        }
        while (read(mouseinputopen,&mouseinputs,sizeof(mouseinputs))==sizeof(mouseinputs)){
            // cout<<mouseinputs.value<<"   "<<mouseinputs.code<<"\n";
            continue;
        }
        string clearcords="\033[2J\033[1;1H"+to_string(ph)+" "+to_string(pw)+" "+to_string(pd);
        write(STDOUT_FILENO, clearcords.data(), clearcords.size());
        std::this_thread::sleep_for(std::chrono::microseconds(50000));
        // ssize_t ie=read(keyinputopen,&keybinputs,sizeof(keybinputs));
        // if(ie!=sizeof(keybinputs)){
        //     continue;
        // }
        // if(keybinputs.type==EV_KEY){
        //     if(keybinputs.value==1){
        //         string wasd=(keybinputs.code==17)?"W":(keybinputs.code==30)?"A":(keybinputs.code==31)?"S":(keybinputs.code==32)?"D":"";
        //         cout<<wasd<<"\n";//doesnt work without <<"\n";
        //     }
        //     else if(keybinputs.value==2){
        //         string wasd=(keybinputs.code==17)?"W":(keybinputs.code==30)?"A":(keybinputs.code==31)?"S":(keybinputs.code==32)?"D":"";
        //         cout<<wasd<<"\n";//doesnt work without <<"\n";
        //     }
        //     else if(keybinputs.value==0){
        //         string wasd=(keybinputs.code==17)?"W":(keybinputs.code==30)?"A":(keybinputs.code==31)?"S":(keybinputs.code==32)?"D":"";
        //         cout<<wasd<<"\n";//doesnt work without <<"\n";
        //     }
        // }

    }
    close(keyinputopen);
    close(mouseinputopen);
}