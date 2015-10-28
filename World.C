#include <cmath>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include "World.H"
#include "Unit.H"

//May not need this inclusion
#include "World2.C"


using namespace std;

/* helper that you need to implement
   
 - move unit by current_speed * heading

 - units only collide with the map border
 
 - when units hit a wall they stop at the collision point (current_speed=0)
   and collision_hook is called with the previous speed and a vector of bools
   encoding the walls that were hit. This vector is used in collision_hook to
   implement bouncing by only changing the unit's heading, i.e. even when
   bouncing the unit stays at the collision location for the remainder of the
   simulation frame and only starts moving again in the following frame.

 - it is essential that units are located inside the map at all times. There
   can be no overlap. Ensure this property by clipping coordinates after
   moving.

*/

void World::move_unit(Unit &u)
{
  //Take unit speed multiply by heading to get new position
  double futureX = u.pos.x + (u.current_speed*u.heading.x);
  double futureY = u.pos.y + (u.current_speed*u.heading.y);  
  //Extra space to leave for a collision. Compensates for rounding errors.
  double buffer = 0.1; 
  //Collision occured flag
  bool collision = false;
  bool collisions[4] = {false, false, false, false};
    //double prev_speed = u.current_speed;

  //Check to see if movement would result in a collision
  if(futureX+u.radius+buffer >= width) {
    //Collision with Right wall
    futureX = width - u.radius - buffer;
    collision = true;
    collisions[1] = true;
  }
  if(futureX-u.radius-buffer <= 0) {
    //Collision with Left wall
    futureX = u.radius + buffer;
    collision = true;
    collisions[3] = true;
  }
  if(futureY+u.radius+buffer >= height) {
    //Collision with Bottom wall
    futureX = height - u.radius - buffer;
    collision = true;
    collisions[2] = true;
  }
  if(futureY-u.radius-buffer >= 0) {
    //Collision with Top wall
    futureX = u.radius + buffer;
    collision = true;  
    collisions[0] = true; 
  }

  if(collision) {
    u.collision_hook(u.current_speed, collisions);
    u.current_speed = 0;
  } 

  //Update unit's position to new position
  u.pos = Vec2(futureX, futureY);

}


// returns policy name
const char *apol2str(AttackPolicy pol)
{
  switch (pol) {
    case ATTACK_WEAKEST:
      return "attack-weakest";
    case ATTACK_CLOSEST:
      return "attack-closest";
    case ATTACK_MOST_DANGEROUS:
      return "attack-most-dangerous";
  }
  return "?";
}


World::~World()
{
  // clean up
  
  // ... implement
}


// deducts hit points and calls attack_hook

void World::attack(Unit &attacker, Unit &attacked)
{
  //Reduce attacked unit's health by attack strength of attacker
  attacked.hp -= (int)attacker.damage;

  //Remove dead is handled by step in World2.C
}

// return a random position at which a circle of that radius fits

Vec2 World::rnd_pos(double radius) const
{
  double slack = radius * 2;

  Vec2 p((width  - 2*slack) * rnd01() + slack,
         (height - 2*slack) * rnd01() + slack);
  
  // assert circle in rectangle
  assert(p.x > radius && p.x < width - radius);
  assert(p.y > radius && p.y < height - radius);
  return p;
}


// return uniform random heading
// length of vector = 1
Vec2 World::rnd_heading() const
{
  // Generate random value between (0 and 2)*pi
  // This should preserve a uniform distribution as the 2*rnd01 
  // transforms from an infinite set of points
  double angle = (this->rnd01())*2; 

  // create Vec2 object with x = cos(angle), y = sin(angle)
  return Vec2(cos(angle), sin(angle));
}

// mirror position with respect to map mid-point
Vec2 World::mirror(const Vec2 &pos) const
{
  //Create Mid-point coordinates
  double midX = width/2;
  double midY = height/2;
  double newX;
  double newY;
  double diff;

  //Set new coordinate to position same distance from midpoint 
  //but on the other side of said midpoint
  if(pos.x >= midX) {
    diff = pos.x - midX;
    newX = midX - diff;
  } else {
    diff = midX - pos.X;
    newX = midX + diff;
  }
  if(pos.y >= midY){
    diff = pos.Y - midY;
    newY = midY - diff;
  } else {
    diff = midY - pos.Y;
    newY = midY + diff;
  }

  //Create vector with new values and return it
  return Vec2(newX, newY);
}


// return squared distance between two unit centers
double World::distance2(const Unit &u, const Unit &v)
{
  return pow((v.x - u.x), 2) + pow((v.y - u.y), 2);
}

// return true iff u can attack v, i.e. distance of u's and v's centers is
// less than u's attack distance plus v's radius
bool World::can_attack(const Unit &u, const Unit &v)
{
  double dist = sqrt(distance2(u, v));

  if(dist <= u.attack_radius + v.radius){
    return true;
  } else {
    return false;  
  }
}

// populate a vector with enemy units that can be attacked by u;
// clears vector first
void World::enemies_within_attack_range(const Unit &u,
                                        vector<Unit*> &targets) const
{
  targets.clear();

  //Append all units to vector 
  for(auto &unit : units) {
    if(can_attack(u, unit)) {
      if(unit->team != u->team){
        targets.push_back(unit);
      }
    }
  }
}

// return a random unit that can be attacked by u with minimal hp_old value,
// or 0 if none exists
Unit *World::random_weakest_target(Unit &u) const
{
  //Create list of possible targets
  vector<Unit*> targets;
  enemies_within_attack_range(u, targets);

  //If there are no available targets
  if(targets.size() ==0) {return 0;}

  //Search for unit with lowest hp
  Unit * lowest = targets[0];
  for(auto &unit: targets) {
    if(unit->hp < lowest->hp){
      lowest = unit;
    }
  }

  return lowest;
}


// return a random unit that can be attacked by u with minimal distance to
// u, or 0 if none exists

Unit *World::random_closest_target(Unit &u) const
{
  //Create list of possible targets
  vector<Unit*> targets;
  enemies_within_attack_range(u, targets);

  //If there are no available targets
  if(targets.size() ==0) {return 0;}

  //Search for unit with lowest distance
  Unit * closest = targets[0];
  for(auto &unit: targets) {
    if(distance2(*u, *unit) < distance2(*u, *closest)){
      closest = unit;
    }
  }

  return closest;
}

// return a random unit that can be attacked by u with maximal damage/hp_old
// ratio, or 0 if none exists
Unit *World::random_most_dangerous_target(Unit &u) const
{
  //Create list of possible targets
  vector<Unit*> targets;
  enemies_within_attack_range(u, targets);

  //If there are no available targets
  if(targets.size() ==0) {return 0;}

  //Search for unit with highest damage
  Unit * baddest = targets[0];
  for(auto &unit: targets) {
    if(unit->damage < lowest->damage){
      baddest = unit;
    }
  }

  return baddest;
}


// return score for red: 2 for win, 0 for loss, 1 for draw, -1 for game not
// over yet
int World::red_score() const
{
  //Flags for if red or blue units still exist
  bool red = false;
  bool blue = false;

  if (units.empty()) {
    return 1;
  }
  
  //Check all remaining units for team affiliations
  for(auto &unit : units){
    if(unit->Team == Unit::Team::BLUE){
      blue = true;
    }
    if(unit->Team == Unit::Team::RED){
      red = true;
    }
    //If there are at least one unit of each team game continues
    if(red && blue){return -1;} 
  }
  
  //If in all the remaining units, none are red loss
  if(!red) {
    return 0;
  } else {
    return 2; //Otherwise return win!
  }


}

