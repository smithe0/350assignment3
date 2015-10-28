#include "Marine.H"

Marine::Marine(Team team, const Vec2 & pos, AttackPolicy pol, bool bounce)
: Unit(team, pos, 10, 40, 10, 1, 45) {
   this->team = team;
   this->pos = pos;
   this->bounce = bounce;
   policy = pol;
}

//Override virtual function in unit
void Marine::act(World &w){
   //Do what you do best
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

   //March
   w.move_unit(*this);
}

//Override virtual function in unit
void Marine::collision_hook(double prev_speed, bool collisions[4]){
   //Reverse direction of heading based on which wall the collision occured at
   if(collisions[0] || collisions[2]){
      heading.y *= -1;
   } 
   
   if(collisions[1] || collisions[3]){
     heading.x *= -1; 
   } 
   
   if(bounce){current_speed = prev_speed;}
}


