//
//  Game.cpp
//  440-Q-Learning-Pong
//
//  Created by Liuyi Shi on 12/5/17.
//  Copyright © 2017 Liuyi Shi. All rights reserved.
//

#include "Game.hpp"
#include <random>

game::Game::Game(){
    paddle_height = 0.2;
    
    ball_x = 0.5;
    ball_y = 0.5;
    velocity_x = 0.03;
    velocity_y = 0.01;
    paddle_y = 0.5 - paddle_height/2;
}

void game::Game::reset(){
    ball_x = 0.5;
    ball_y = 0.5;
    velocity_x = 0.03;
    velocity_y = 0.01;
    paddle_y = 0.5 - paddle_height/2;
}

void game::Game::move_ball(){
    ball_x += velocity_x;
    ball_y += velocity_y;
}
bool game::Game::bounce(){
    if(ball_y < 0){
        ball_y = -1 * ball_y;
        velocity_y = -1 * velocity_y;
    }
    else if(ball_y > 1){
        ball_y = 2 - ball_y;
        velocity_y = -1 * velocity_y;
    }
    if(ball_x < 0){
        ball_x = -1 * ball_x;
        velocity_x = -1 * velocity_x;
    }
    else if(ball_x > 1){
        if(ball_y >= paddle_y && ball_y <= paddle_y + paddle_height){
            ball_x = 2 - ball_x;
            velocity_x = -1 * velocity_x + (rand() % 3001 - 1500)/100000.0;
            if( abs(velocity_x) < 0.03 ){
                if( velocity_x != 0 )
                    velocity_x = 0.03 * (velocity_x/abs(velocity_x));
                else
                    velocity_x = (rand()%2 == 0)?0.03:(-0.03);
            }
            else if( abs(velocity_x) > 1 )
                velocity_x = 1 * (velocity_x/abs(velocity_x));
            
            velocity_y = velocity_y + (rand() % 6001 - 3000)/100000.0;
            if( abs(velocity_y) > 1 )
                velocity_y = 1 * (velocity_y/abs(velocity_y));
            
            return true;
        }
    }
    return false;
}

bool game::Game::is_termination(){
    if(ball_x > 1)
        return true;
    else
        return false;
}

void game::Game::output_status(){
    printf("b_x:%f b_y:%f v_x:%f v_y:%f p_y:%f\n", ball_x, ball_y, velocity_x, velocity_y, paddle_y);
}

void game::Game::move_paddle(Action_Set a){
    if(a == Up)
        paddle_y -= 0.04;
    else if(a == Down)
        paddle_y += 0.04;
    
    if( paddle_y<0 )
        paddle_y = 0;
    else if( paddle_y > 1-paddle_height )
        paddle_y = 1-paddle_height;

}

int game::Game::play_a_round(){
    int num = 0;
    while(1){
        move_paddle(choose_action());
        move_ball();
        if(bounce())
            num++;
        if(is_termination()){
            reset();
            return num;
        }
    }
    return 0;
}

unsigned int game::Game::get_state(unsigned int board_discretion, unsigned int paddle_discretion){
    unsigned int b_x_discrete;
    unsigned int b_y_discrete;
    int v_x_discrete;
    int v_y_discrete;
    unsigned int p_y_disctete;
    bool is_special_state;
    
    unsigned int state = 0;

    is_special_state = ball_x > 1;
    
    //discrete ball_x between 0 to board_discretion-1
    b_x_discrete = floor(ball_x*board_discretion);
    if(b_x_discrete == board_discretion)
        b_x_discrete = board_discretion-1;
    //discrete ball_y between 0 to board_discretion-1
    b_y_discrete = floor(ball_y*board_discretion);
    if(b_y_discrete == board_discretion)
        b_y_discrete = board_discretion-1;
    //discrete velocity_x between -1 and 1
    v_x_discrete = velocity_x/abs(velocity_x);
    //discrete velocity_y between -1,0,1
    if( abs(velocity_y) < 0.015 )
        v_y_discrete = 0;
    else
        v_y_discrete = velocity_y/abs(velocity_y);
    //discrete paddle_y between 0 to paddle_discretion-1
    p_y_disctete = floor(paddle_discretion*paddle_y/(1-paddle_height));
    if(p_y_disctete == paddle_discretion)
        p_y_disctete = paddle_discretion-1;
    if(is_special_state)
        state = 0;
    else{
        state = 0;
        state += (b_y_discrete*board_discretion + b_x_discrete) * 2 * 3 * paddle_discretion;
        state += p_y_disctete * 2 * 3;
        state += (v_y_discrete + 1) * 2;
        state += (v_x_discrete==-1)?0:1;
        state += 1;
    }
    return state;
}

unsigned int game::Game::get_state_size(unsigned int board_discretion, unsigned int paddle_discretion){
    return (board_discretion*board_discretion*paddle_discretion*2*3+1);
}

int game::Game::get_reward(){
    int reward;
    if(ball_x > 1){
        if(ball_y >= paddle_y && ball_y <= paddle_y + paddle_height)
            reward = 1;
        else
            reward = -1;
    }
    else
        reward = 0;
    return reward;
}

void game::Game::Q_init(){
    //init Q, total size if action_number * state_size, each entry for Q(s,a)
    Q.resize(get_state_size(12, 12));
    for(auto &i : Q)
        i.resize(3);
    N.resize(get_state_size(12, 12));
    for(auto &i : N)
        i.resize(3);
}

float f(float q,int n){
    if(n<100)
        return 3;
    else
        return q;
}

game::Action_Set game::Game::exploration(float e){
    Action_Set a = Nothing;
//    if( (rand()%100)/100.0 < e){
//        a = static_cast<Action_Set>(rand() % 3);
//    }
//    else{
//        if( Q[get_state(12, 12)][Up] > Q[get_state(12, 12)][a])
//            a = Up;
//        if( Q[get_state(12, 12)][Down] > Q[get_state(12, 12)][a])
//            a = Down;
//    }
    
    if( f(Q[get_state(12, 12)][Up],N[get_state(12, 12)][Up]) > f(Q[get_state(12, 12)][a],N[get_state(12, 12)][a]))
        a = Up;
    if( f(Q[get_state(12, 12)][Down],N[get_state(12, 12)][Down]) > f(Q[get_state(12, 12)][a],N[get_state(12, 12)][a]))
        a = Down;
    if(f(Q[get_state(12, 12)][Up],N[get_state(12, 12)][Up]) == f(Q[get_state(12, 12)][Down],N[get_state(12, 12)][Down]) && f(Q[get_state(12, 12)][Down],N[get_state(12, 12)][Down]) == f(Q[get_state(12, 12)][Nothing],N[get_state(12, 12)][Nothing]) ){
        int r = rand()%3;
        a = static_cast<Action_Set>(r);
    }
    return a;
}

game::Action_Set game::Game::choose_action(){
    Action_Set a = Nothing;
    if( Q[get_state(12, 12)][Up] > Q[get_state(12, 12)][a])
        a = Up;
    if( Q[get_state(12, 12)][Down] > Q[get_state(12, 12)][a])
        a = Down;
    if( Q[get_state(12, 12)][Up] == Q[get_state(12, 12)][Down] && Q[get_state(12, 12)][Down] == Q[get_state(12, 12)][Nothing])
        a = static_cast<Action_Set>(rand()%3);
    return a;
}

float game::Game::get_utility(unsigned int state){
    float u = -9999999;
    if(Q[state][Nothing] > u)
        u = Q[state][Nothing];
    if(Q[state][Up] > u)
        u = Q[state][Up];
    if(Q[state][Down] > u)
        u = Q[state][Down];
    return u;
}

void game::Game::train(){
    float alpha = 1;
    float gamma = 0.1;
    unsigned int s_current = get_state(12, 12);
    int R = get_reward();
    Action_Set a = choose_action();
    move_paddle(a);
    move_ball();
    bounce();
    unsigned int s_next = get_state(12, 12);

    Q[s_current][a] = Q[s_current][a] + alpha * (R + gamma * get_utility(get_state(12, 12)) - Q[s_current][a]);
}

void game::Game::train_a_round(){
    unsigned int C = 200;
    float alpha;
    float gamma = 0.9;
    
    unsigned int s_current;
    int R_current = 0;
    int R_next;
    Action_Set a = Nothing;
    unsigned int s_next;

    while(1){
        s_current = get_state(12, 12);
        a = exploration(0.2);
        alpha = (float)C/(C+N[s_current][a]);
        
        N[s_current][a]++;
        move_paddle(a);
        move_ball();
        R_next = get_reward();
        bounce();
        s_next = get_state(12, 12);
        
        Q[s_current][a] = Q[s_current][a] + alpha * (R_current + gamma * get_utility(s_next) - Q[s_current][a]);
        R_current = R_next;
        
        if(is_termination()){
            reset();
            break;
        }
    }
}

void game::Game::test(){
    float maxQ = -9999;
    for(auto &i:Q){
        for(auto &j:i){
            if(j > maxQ)
                maxQ = j;
        }
    }
    printf("%f\n",maxQ);
}
