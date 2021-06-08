/*    Author: Anto Murugan
 *    Lab Section: 021
 *    Assignment: Lab 11  
 *    Exercise Description: Classic Pong Game
 *
 *    I acknowledge all content contained herein, excluding template or example
 *    code, is my own original work.
 *
 *    Demo Link: https://www.youtube.com/watch?v=BnB3eD1Xvxw&ab_channel=GowthamAnto
 */

#include <avr/io.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//---------------------------
//global variables
//---------------------------
// Button detection
unsigned char button;
// Reset game
unsigned int r1 = 0;
unsigned int r2 = 0;
unsigned int r = 0;
// Positions
unsigned char bx = 0x10;
unsigned char by = 0xFB;
unsigned char p1X = 0x80;
unsigned char p2X = 0x01;
unsigned char p1Y = 0xF1;
unsigned char p2Y = 0xF1;
unsigned char p1_1 = 0xFD;
unsigned char p1_2 = 0xFB;
unsigned char p1_3 = 0xF7;
unsigned char p2_1 = 0xFD;
unsigned char p2_2 = 0xFB;
unsigned char p2_3 = 0xF7;
unsigned int moveUp = 0;
unsigned int moveDown = 0;
// Scores
unsigned char p1 = 0x00;
unsigned char p2 = 0x00;
//---------------------------
// Display States and Controls
//---------------------------
enum d_states {Init_d, p1_d, p2_d, b_d, p1_scores, p2_scores};
int display_tick(int state) {
    switch(state) {
      case Init_d:
          bx = 0x10;
          by = 0xFB;
          p1X = 0x80;
          p2X = 0x01;
          p1Y = 0xF1;
          p2Y = 0xF1;
          p1_1 = 0xFD;
          p1_2 = 0xFB;
          p1_3 = 0xF7;
          p2_1 = 0xFD;
          p2_2 = 0xFB;
          p2_3 = 0xF7;
          r = 0;
          state = p1_d;
          break;
      case p1_d:
         if(r > 0) {
            state = Init_d;
            break;
         }
         if(r2 > 0) {
            p1 = 0;
            p2 = 0;
            r2 = 0;
         }
         if((p1 == 0x07) || (p2 == 0x07)) {
            if(p1 == 0x07) {
                state = p1_scores;
                break;
            }
            if(p2 == 0x07) {
                state = p2_scores;
                break;
            }
         }
         PORTA = p1 | (p2 << 3);
         PORTC = p1X;
         PORTD = p1Y;
         state = p2_d;
         break;
      case p2_d:
         if(r > 0) {
            state = Init_d;
            break;
         }
         if(r2 > 0) {
            p1 = 0;
            p2 = 0;
            r2 = 0;
         }
         if((p1 == 0x07) || (p2 == 0x07)) {
            if(p1 == 0x07) {
                state = p1_scores;
                break;
            }
            if(p2 == 0x07) {
                state = p2_scores;
                break;
            }
         }
         PORTA = p1 | (p2 << 3);
         PORTC = p2X;
         PORTD = p2Y;
         state = b_d;
         break;
      case b_d:
         if(r > 0) {
            state = Init_d;
            break;
         }
         if(r2 > 0) {
            p1 = 0;
            p2 = 0;
            r2 = 0;
         }
         PORTA = p1 | (p2 << 3);
         PORTC = bx;
         PORTD = by;
         state = p1_d;
         break;
      case p1_scores:
         p1 = 0;
         p2 = 0;
         state = Init_d;
         break;
      case p2_scores:
         p1 = 0;
         p2 = 0;
         state = Init_d;
         break;
    }
    return state;
}

enum ball_states {Init, Right, Left};
int ball_tick(int state) {
    switch(state) {
      case Init:
          if(r1 > 0) {
            bx = 0x10;
            state = Init;
            r1 = 0;
            moveUp = 0;
            moveDown = 0;        
          }
          if((bx << 1) != 0x80) {
            bx = bx << 1;
            state = Init;
          }
          else {
            bx = bx << 1;
            if (by == p1_2) {
                bx = bx >> 1;
                state = Right;
                break;
            }
            else if (by == p1_1) {
                bx = bx >> 1;
                if (by == 0xFE) {
                  moveDown = 1;
                  moveUp = 0;
                  by = (by << 1) | 0x01;
                }
                else {
                  by = (by >> 1) | 0x80;
                  moveDown = 0;
                  moveUp = 1;
                }
                state = Right;
                break;
            }
            else if (by == p1_3) {
                bx = bx >> 1;
                if (by == 0xEF) {
                  by = (by >> 1) | 0x80;
                  moveDown = 0;
                  moveUp = 1;
                }
                else {              
                  by = (by << 1) | 0x01;
                  moveUp = 0;
                  moveDown = 1;
                }
                state = Right;
                break;
            }
            else {
                if (p2 == 0x00) {
                  p2 = p2 + 1;
                }
                else {
                  p2 = (p2 << 1) + 1;
                }
                r = 1;
                r1 = 1;
            }
          }     
          break;
      case Right:
          if(r1 > 0) {
            bx = 0x10;
            state = Init;
            r1 = 0;
            moveUp = 0;
            moveDown = 0;  
            break;
          }
          if((bx >> 1) != 0x01) {
            if ((moveUp == 0) && (moveDown == 0)) {
                bx = bx >> 1;
                state = Right;
            }
            else if (moveUp == 1) {
                bx = bx >> 1;
                if(by == 0xFE) {
                  by = (by << 1) | 0x01;
                  moveUp = 0;
                  moveDown = 1;
                }
                else {
                  by = (by >> 1) | 0x80;
                }
            }
            else if (moveDown == 1) {
                bx = bx >> 1;
                if(by == 0xEF) {
                  moveUp = 1;
                  moveDown = 0;
                  by = (by >> 1) | 0x80;
                }
                else {
                  by = (by << 1) | 0x01;
                }
            }
          }
          else {
            bx = bx >> 1;
            if (by == p2_2) {
                bx = bx << 1;
                state = Left;
                break;
            }
            else if (by == p2_1) {
                bx = bx << 1;
                if (by == 0xFE) {
                  moveDown = 1;
                  moveUp = 0;
                  by = (by << 1) | 0x01;
                }
                else {
                  by = (by >> 1) | 0x80;
                  moveDown = 0;
                  moveUp = 1;
                }
                state = Left;
                break;
            }
            else if (by == p2_3) {
                bx = bx << 1;
                if (by == 0xEF) {
                  by = (by >> 1) | 0x80;
                  moveDown = 0;
                  moveUp = 1;
                }
                else {              
                  by = (by << 1) | 0x01;
                  moveUp = 0;
                  moveDown = 1;
                }
                state = Left;
                break;
            }
            else {
                if (p1 == 0x00) {
                  p1 = p1 + 1;
                }
                else {
                  p1 = (p1 << 1) + 1;
                }
                r = 1;
                r1 = 1;
            }
          }
          break;
      case Left:
          if(r1 > 0) {
            bx = 0x10;
            state = Init;
            r1 = 0;
            moveUp = 0;
            moveDown = 0;  
            break;
          }
          if((bx << 1) != 0x80) {
            if ((moveUp == 0) && (moveDown == 0)) {
                bx = bx << 1;
                state = Left;
            }
            else if (moveUp == 1) {
                bx = bx << 1;
                if(by == 0xFE) {
                  by = (by << 1) | 0x01;
                  moveUp = 0;
                  moveDown = 1;
                }
                else {
                  by = (by >> 1) | 0x80;
                }
            }
            else if (moveDown == 1) {
                bx = bx << 1;
                if(by == 0xEF) {
                  moveUp = 1;
                  moveDown = 0;
                  by = (by >> 1) | 0x80;
                }
                else {
                  by = (by << 1) | 0x01;
                }
            }
          }
          else {
            bx = bx << 1;
            if (by == p1_2) {
                bx = bx >> 1;
                state = Right;
                break;
            }
            else if (by == p1_1) {
                bx = bx >> 1;
                if (by == 0xFE) {
                  moveDown = 1;
                  moveUp = 0;
                  by = (by << 1) | 0x01;
                }
                else {
                  by = (by >> 1) | 0x80;
                  moveDown = 0;
                  moveUp = 1;
                }
                state = Right;
                break;
            }
            else if (by == p1_3) {
                bx = bx >> 1;
                if (by == 0xEF) {
                  by = (by >> 1) | 0x80;
                  moveDown = 0;
                  moveUp = 1;
                }
                else {              
                  by = (by << 1) | 0x01;
                  moveUp = 0;
                  moveDown = 1;
                }
                state = Right;
                break;
            }
            else {
                if (p2 == 0x00) {
                  p2 = p2 + 1;
                }
                else {
                  p2 = (p2 << 1) + 1;
                }
                r = 1;
                r1 = 1;
            }
          }
          break;
    }
    return state;
}

enum p1_states {Wait_p1};
int p1_tick(int state) {
    button = PINB ^ 0xFF;
    button = button & 0xFF;

    switch(state) {
      case Wait_p1:
         if(button == 0x40) {
            r = 1;
            r2 = 1;
            r1 = 1;
         }
         else if(button == 0x02) {
            if(p1Y != 0xF8) {
                p1Y = ((p1Y >> 1 | 0x80));
                p1_1 = ((p1_1 >> 1 | 0x80));
                p1_2 = ((p1_2 >> 1 | 0x80));
                p1_3 = ((p1_3 >> 1 | 0x80));
            }
         }
         else if(button == 0x01) {
            if(p1Y != 0xE3) {
                p1Y = ((p1Y << 1) | 0x01);
                p1_1 = ((p1_1 << 1 | 0x01));
                p1_2 = ((p1_2 << 1 | 0x01));
                p1_3 = ((p1_3 << 1 | 0x01));
            }
         }
         break;
    } 
    return state;
}

enum p2_states {Wait_p2, Move_p2};
int p2_tick(int state) {
    switch(state) {
      case Wait_p2:
          if((bx < 0x10) && (bx > 0x02)) {
            state = Move_p2;
          }
          break;
      case Move_p2:
          if((rand() % 2) == 1) {
            if((rand() % 2) == 1) {
                if(p2Y != 0xF8) {
                  p2Y = ((p2Y >> 1 | 0x80));
                  p2_1 = ((p2_1 >> 1 | 0x80));
                  p2_2 = ((p2_2 >> 1 | 0x80));
                  p2_3 = ((p2_3 >> 1 | 0x80));
                }
            }
            else {
                if(p2Y != 0xE3) {
                  p2Y = ((p2Y << 1) | 0x01);
                  p2_1 = ((p2_1 << 1 | 0x01));
                  p2_2 = ((p2_2 << 1 | 0x01));
                  p2_3 = ((p2_3 << 1 | 0x01));
                }
            }
          }
          state = Wait_p2;
          break;
    }
    return state;
}

int main() {
      
      DDRB = 0x00; PORTB = 0xFF; // Configure port B's pins as input
      DDRA = 0xFF; PORTA = 0x00; // Configure port A's pins as outputs, initialize to 0s  
      DDRC = 0xFF; PORTC = 0x00; // Configure port C's pins as outputs, initialize to 0s 
      DDRD = 0xFF; PORTD = 0x00; // Configure port D's pins as outputs, initialize to 0s

      static task task1, task2, task3, task4;
      task *tasks[] = { &task1, &task2, &task3, &task4 };
      const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

      task1.state = Init;
      task1.period = 125;
      task1.elapsedTime = task1.period;
      task1.TickFct = &ball_tick;

      task2.state = Wait_p1;
      task2.period = 125;
      task2.elapsedTime = task2.period;
      task2.TickFct = &p1_tick;

      task3.state = Wait_p2;
      task3.period = 125;
      task3.elapsedTime = task3.period;
      task3.TickFct = &p2_tick;

      task4.state = Init_d;
      task4.period = 5;
      task4.elapsedTime = task4.period;
      task4.TickFct = &display_tick;

      unsigned long GCD = tasks[0]->period;
      unsigned short i;
      for ( i = 1; i < numTasks; i++ ) {
        GCD = findGCD(GCD, tasks[i]->period);
      }

      TimerSet(GCD);
      TimerOn();

      while (1) {
            for (i = 0; i < numTasks; i++) {
                  if (tasks[i]->elapsedTime >= tasks[i]->period) {
                  tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                  tasks[i]->elapsedTime = 0;
                  }
                  tasks[i]->elapsedTime += GCD;
            }
            while(!TimerFlag);
            TimerFlag = 0;
      }
      
      return 0;
}
