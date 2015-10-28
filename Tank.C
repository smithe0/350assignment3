#include "Tank.H"

Tank::Tank(Team team, const Vec2 & pos, AttackPolicy pol, bool bounce_) 
: Unit(team, pos, 20, 80, 15, 4, 100) {
   this->team = team;
   this->pos = pos;
   bounce = bounce_;
   policy = pol;
}

//Override virtual function in unit
void Tank::act(World &w){
   //Do what tanks do best
   Unit * target;
   switch(policy){
      case ATTACK_WEAKEST:
         target = w.random_weakest_target(*this);
         break;
      case ATTACK_CLOSEST:
         target = w.random_closest_target(*this);
         break;
      case ATTACK_MOST_DANGEROUS:
         target = w.random_most_dangerous_target(*this);
      w.attack(*this, *target);
   }
   
   //Roll out
   w.move_unit(*this);
}

//Override virtual function in unit
void Tank::collision_hook(double prev_speed, bool collisions[4]){
   //Reverse direction of heading based on which wall the collision occured at
   if(collisions[0] || collisions[2]){
      heading.y *= -1;
   } 
   
   if(collisions[1] || collisions[3]){
     heading.x *= -1; 
   } 
   
   if(bounce){current_speed = prev_speed;}
}


