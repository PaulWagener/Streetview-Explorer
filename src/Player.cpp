#include "Player.h"
#include "gl.h"
#include "common.h"

#include <math.h>
#include <stdlib.h>

Player::Player() {
    keys.forward = false;
    keys.backward = false;
    keys.rotate_left = false;
    keys.rotate_right = false;
    keys.strafe_left = false;
    keys.strafe_right = false;
    head = NULL;

    cam.target_elevation = 25;
    cam.elevation = cam.target_elevation;

    cam.target_distance = 12;
    cam.rotation = 0;
    cam.x = 0;
    cam.y = 0;
    cam.z = 0;

    target_rotation = 0;
    rotation = target_rotation;
    target_height = -3;
    height = target_height;

}

Player::~Player() {
    if(head != NULL)
        gluDeleteQuadric(head);
}

/**
 * Allow player to rotate the camera and the player with the mouse
 * 
 * @param x, y  delta of mouse
 */
void Player::moveMouse(int x, int y) {
    target_rotation += x / (float)5;
    cam.target_elevation += y / (float)10;

    const float MAX_CAM_ELEVATION = 70;
    const float MIN_CAM_ELEVATION = -80;
    if (cam.target_elevation > MAX_CAM_ELEVATION)
        cam.target_elevation = MAX_CAM_ELEVATION;
    if (cam.target_elevation < MIN_CAM_ELEVATION)
        cam.target_elevation = MIN_CAM_ELEVATION;
}

/**
 * Let the player zoom in and out with the scroll wheel
 * 
 * @param delta
 */
void Player::mouseWheel(int delta) {
    const int MIN_ZOOM = 7;
    const int MAX_ZOOM = 50;
    const float ZOOM_RATE = 0.1f;

    cam.target_distance -= delta * ZOOM_RATE;

    if(cam.target_distance > MAX_ZOOM) cam.target_distance = MAX_ZOOM;
    if(cam.target_distance < MIN_ZOOM) cam.target_distance = MIN_ZOOM;
}

void Player::initializeLocation(struct utmPosition position) {
    target_location = position;
    location = position;
    cam.location = position;
}

void Player::targetCamera(struct utmPosition reference) {
    //Set the camera behind the player
    cam.location = location;
    
    cam.elevation = cam.target_elevation + (cam.elevation - cam.target_elevation) / 1.3f;
    cam.distance = cam.target_distance + (cam.distance - cam.target_distance) / 1.1f;
    rotation = target_rotation + (rotation - target_rotation) / 1.18f;
    height = target_height + (height - target_height) / 1.1f;
    
    cam.location.northing = location.northing + cos((180 + rotation) * RADIAL) * cos(cam.elevation * RADIAL) * cam.distance;
    cam.location.easting = location.easting + sin((180 + rotation) * RADIAL) * cos(cam.elevation * RADIAL) * cam.distance;

    //Work out the camera position in local OpenGL coordinates
    cam.x = (float)(cam.location.easting - reference.easting);
    cam.y = (float)(cam.location.northing - reference.northing);
    
    float target_z = sin(cam.elevation * RADIAL) * cam.target_distance + target_height;

    //Skimmy along the ground if the camera is looking up
    if (target_z < target_height + 1)
        target_z = target_height + 1;

    cam.z = target_z + (cam.z - target_z) / 1.2f;

    gluLookAt(cam.x, cam.y, cam.z,
            location.easting - reference.easting, location.northing - reference.northing, height + 4, //Look above the player's head
            0, 0, 1);

}

/**
 * Process input from keys
 */
void Player::updatePosition() {
    const float FORWARD_SPEED = 0.18f;
    const float ROTATION_SPEED = 2.0f;
    
    /**
     * Update player position
     */
    float forward_speed = 0;
    float right_speed = 0;
    if (keys.forward) forward_speed = FORWARD_SPEED;
    if (keys.backward) forward_speed = -FORWARD_SPEED;
    if (keys.strafe_right) right_speed = FORWARD_SPEED;
    if (keys.strafe_left) right_speed = -FORWARD_SPEED;
    if (keys.rotate_left) target_rotation -= ROTATION_SPEED;
    if (keys.rotate_right) target_rotation += ROTATION_SPEED;


    target_location.northing += cos(target_rotation * RADIAL) * forward_speed;
    target_location.easting += sin(target_rotation * RADIAL) * forward_speed;

    target_location.northing += cos((target_rotation + 90) * RADIAL) * right_speed;
    target_location.easting += sin((target_rotation + 90) * RADIAL) * right_speed;


    //Go to actual player location
    location.easting = target_location.easting + (location.easting - target_location.easting) / 1.2;
    location.northing = target_location.northing + (location.northing - target_location.northing) / 1.2;

}

/**
 * Draw a box around a point
 * 
 * @param width,depth,height  dimensions of box
 */
void Player::drawBox(const float width, const float depth, const float height) {
    const float half_height = height / 2;
    const float half_width = width / 2;
    const float half_depth = depth / 2;

    glBegin(GL_QUADS);

    //Bottom
    glNormal3f(0, 0, -1);
    glVertex3f(-half_width, -half_depth, -half_height);
    glVertex3f(-half_width, half_depth, -half_height);
    glVertex3f(half_width, half_depth, -half_height);
    glVertex3f(half_width, -half_depth, -half_height);

    //Top
    glNormal3f(0, 0, 1);
    glVertex3f(half_width, -half_depth, half_height);
    glVertex3f(half_width, half_depth, half_height);
    glVertex3f(-half_width, half_depth, half_height);
    glVertex3f(-half_width, -half_depth, half_height);

    //Left
    glNormal3f(-1, 0, 0);
    glVertex3f(-half_width, -half_depth, half_height);
    glVertex3f(-half_width, half_depth, half_height);
    glVertex3f(-half_width, half_depth, -half_height);
    glVertex3f(-half_width, -half_depth, -half_height);

    //Right
    glNormal3f(1, 0, 0);
    glVertex3f(half_width, -half_depth, -half_height);
    glVertex3f(half_width, half_depth, -half_height);
    glVertex3f(half_width, half_depth, half_height);
    glVertex3f(half_width, -half_depth, half_height);

    //Back
    glNormal3f(0, -1, 0);
    glVertex3f(-half_width, -half_depth, -half_height);
    glVertex3f(half_width, -half_depth, -half_height);
    glVertex3f(half_width, -half_depth, half_height);
    glVertex3f(-half_width, -half_depth, half_height);

    //Front
    glNormal3f(0, 1, 0);
    glVertex3f(-half_width, half_depth, half_height);
    glVertex3f(half_width, half_depth, half_height);
    glVertex3f(half_width, half_depth, -half_height);
    glVertex3f(-half_width, half_depth, -half_height);

    glEnd();
}

/**
 * Draw the player representation
 * It is supposed to look like the orange streetview guy that you drag across the map
 */
void Player::drawPlayer(struct utmPosition reference) {

    glPushMatrix();
    glTranslated(location.easting - reference.easting, location.northing - reference.northing, height);
    glRotatef(target_rotation, 0, 0, -1);

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
    glColor4d(1, 0.6, 0, 0.8); //Streetview Orange

    const float SHOULDER_HEIGHT = 1.6f;
    const float BODY_WIDTH = 0.6f;
    const float BODY_DEPTH = 0.4f;
    const float HEAD_RADIUS = 0.3f;
    const float ARM_WIDTH = 0.2f;
    const float ARM_LENGTH = 0.85f;
    const float RAYMAN_LENGTH = 0.05f;

    glPushMatrix();
    {
        //Body
        glTranslatef(0, 0, SHOULDER_HEIGHT / 2);
        drawBox(BODY_WIDTH, BODY_DEPTH, SHOULDER_HEIGHT);

        //Head
        if(head == NULL)
            head = gluNewQuadric();
        
        glTranslatef(0, 0, SHOULDER_HEIGHT / 2 + RAYMAN_LENGTH + HEAD_RADIUS);
        gluSphere(head, HEAD_RADIUS, 8, 8);
    }
    glPopMatrix();

    //Right arm
    glPushMatrix();
    {        
        glTranslatef(RAYMAN_LENGTH + (BODY_WIDTH / 2) + (ARM_WIDTH / 2), 0, SHOULDER_HEIGHT - (ARM_LENGTH / 2));
        drawBox(ARM_WIDTH, BODY_DEPTH, ARM_LENGTH);
    }
    glPopMatrix();

    //Left arm
    glPushMatrix();
    {        
        glTranslatef(-RAYMAN_LENGTH - (BODY_WIDTH / 2) - (ARM_WIDTH / 2), 0, SHOULDER_HEIGHT - (ARM_LENGTH / 2));
        drawBox(ARM_WIDTH, BODY_DEPTH, ARM_LENGTH);
    }
    glPopMatrix();


    glPopMatrix();
    glDisable(GL_LIGHTING);
}