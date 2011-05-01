#include "Player.h"
#include "gl.h"
#include "common.h"

#include <math.h>

Player::Player() {
    keys.forward = false;
    keys.backward = false;
    keys.rotate_left = false;
    keys.rotate_right = false;
    keys.strafe_left = false;
    keys.strafe_right = false;
}

void Player::moveMouse(int x, int y) {
    rotation += x / (float) 4;
    cam.elevation += y / (float) 4;

    const float MAX_CAM_ELEVATION = 90;
    const float MIN_CAM_ELEVATION = -90;
    if (cam.elevation > MAX_CAM_ELEVATION)
        cam.elevation = MAX_CAM_ELEVATION;
    if (cam.elevation < MIN_CAM_ELEVATION)
        cam.elevation = MIN_CAM_ELEVATION;
}

#include <math.h>

void Player::initializeLocation(struct utmPosition position) {
    target_location = position;
    location = position;
    cam.location = position;
    
    cam.elevation = 5;
    cam.distance = 12;
    cam.rotation = 0;
    cam.x = 0;
    cam.y = 0;
    cam.z = 0;

    rotation = 0;
    height = -3;
}

void Player::targetCamera(struct utmPosition reference) {
    //Set the camera behind the player
    cam.location = location;
    cam.location.northing = location.northing + cos((180 + rotation) * RADIAL) * cos(cam.elevation * RADIAL) * cam.distance;
    cam.location.easting = location.easting + sin((180 + rotation) * RADIAL) * cos(cam.elevation * RADIAL) * cam.distance;

    //Work out the camera position in local OpenGL coordinates
    const float target_x = (float)(cam.location.easting - reference.easting);
    const float target_y = (float)(cam.location.northing - reference.northing);
    float target_z = sin(cam.elevation * RADIAL) * cam.distance + height;

    //Skimmy along the ground if the camera is looking up
    if (target_z < height + 1)
        target_z = height + 1;

    //Pivot camera around player location
    cam.x = target_x + (cam.x - target_x) / 1.2f;
    cam.y = target_y + (cam.y - target_y) / 1.1f;
    cam.z = target_z + (cam.z - target_z) / 1.1f;

    gluLookAt(cam.x, cam.y, cam.z,
            location.easting - reference.easting, location.northing - reference.northing, height + 4, //Look above the player's head
            0, 0, 1);

}

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
    if (keys.rotate_left) rotation -= ROTATION_SPEED;
    if (keys.rotate_right) rotation += ROTATION_SPEED;


    target_location.northing += cos(rotation * RADIAL) * forward_speed;
    target_location.easting += sin(rotation * RADIAL) * forward_speed;

    target_location.northing += cos((rotation + 90) * RADIAL) * right_speed;
    target_location.easting += sin((rotation + 90) * RADIAL) * right_speed;


    //Go to actual player location
    location.easting = target_location.easting + (location.easting - target_location.easting) / 1.2;
    location.northing = target_location.northing + (location.northing - target_location.northing) / 1.2;

}

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
    glRotatef(rotation, 0, 0, -1);

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
        glTranslatef(0, 0, SHOULDER_HEIGHT / 2 + RAYMAN_LENGTH + HEAD_RADIUS);
        glutSolidSphere(HEAD_RADIUS, 8, 8);
    }
    glPopMatrix();

    glPushMatrix();
    {
        //Right arm
        glTranslatef(RAYMAN_LENGTH + (BODY_WIDTH / 2) + (ARM_WIDTH / 2), 0, SHOULDER_HEIGHT - (ARM_LENGTH / 2));
        drawBox(ARM_WIDTH, BODY_DEPTH, ARM_LENGTH);
    }
    glPopMatrix();

    glPushMatrix();
    {
        //Left arm
        glTranslatef(-RAYMAN_LENGTH - (BODY_WIDTH / 2) - (ARM_WIDTH / 2), 0, SHOULDER_HEIGHT - (ARM_LENGTH / 2));
        drawBox(ARM_WIDTH, BODY_DEPTH, ARM_LENGTH);
    }
    glPopMatrix();


    glPopMatrix();
    glDisable(GL_LIGHTING);
}