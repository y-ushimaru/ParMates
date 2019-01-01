/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "SignalColor.h"

SignalMainState SignalColor::_blue = 1;
SignalMainState SignalColor::_red = 2;
SignalMainState SignalColor::_yellow = 3;
SignalMainState SignalColor::_redblink = 4;
SignalMainState SignalColor::_yellowblink = 5;

SignalSubState SignalColor::_none = 0;
SignalSubState SignalColor::_all = 123;
SignalSubState SignalColor::_straight = 1;
SignalSubState SignalColor::_left = 2;
SignalSubState SignalColor::_right = 3;
SignalSubState SignalColor::_straightLeft  = 12;
SignalSubState SignalColor::_straightRight = 13;
SignalSubState SignalColor::_leftRight = 23;

SignalColor::SignalColor(){}

SignalColor::~SignalColor(){}

// main color
SignalMainState SignalColor::blue(){ return _blue;}
SignalMainState SignalColor::red(){ return _red;}
SignalMainState SignalColor::yellow(){return _yellow;}
SignalMainState SignalColor::redblink(){ return _redblink;}
SignalMainState SignalColor::yellowblink(){return _yellowblink;}

// sub color
SignalSubState SignalColor::none(){return _none;}
SignalSubState SignalColor::all(){return _all;}
SignalSubState SignalColor::straight(){return _straight;}
SignalSubState SignalColor::left(){return _left;}
SignalSubState SignalColor::right(){return _right;}
SignalSubState SignalColor::straightLeft(){return _straightLeft;}
SignalSubState SignalColor::straightRight(){return _straightRight;}
SignalSubState SignalColor::leftRight(){return _leftRight;}
