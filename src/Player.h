/* 
 * File:   Player.h
 * Author: paulwagener
 *
 * Created on 9 april 2011, 13:17
 */

#include "Utm.h"
#include "gl.h"

#ifndef PLAYER_H
#define	PLAYER_H

class Player {
public:
    GLUquadric *head;

    struct keys {
        bool forward;
        bool backward;
        bool strafe_left;
        bool strafe_right;
        bool rotate_left;
        bool rotate_right;
    } keys;

    //Player position
    struct utmPosition target_location;
    struct utmPosition location;
    float target_rotation;
    float rotation;
    float height;

    //Camera position
    struct cam {
        struct utmPosition location;
        float elevation, target_elevation;
        float distance;
        float rotation;
        float x,y,z;
    } cam;


    Player();
    ~Player();
    void drawBox(const float width, const float depth, const float height);
    void drawPlayer(struct utmPosition reference);
    void moveMouse(int x, int y);
    void initializeLocation(struct utmPosition position);
    void updatePosition();

    void targetCamera(struct utmPosition reference);
private:

};

#endif	/* PLAYER_H */

