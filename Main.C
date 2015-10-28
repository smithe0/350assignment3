#include <cassert>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sstream>
#include <cstring>
#include <chrono>
#include <fstream>
//#include <GL/glut.h>
#include "World.H"
#include "Unit.H"
#include "Marine.H"
#include "Tank.H"


using namespace std;

int main(int argc, char *argv[])
{
  if (argc != 1 && argc != 10) {
    cerr << "usage: " << argv[0] << " width height delay seed marines tanks redpol bluepol bounce" << endl;
    cerr << "       delay in ms, seed=0: use time, policies: 0 weakest/1 closest/2 most dangerous" << endl;
    exit(1);
  }

  double width  = 700;
  double height = 700;

  int delay = 100;     // frame delay in ms
  int seed = 0;    // for rng, 0: use time

  int n_marines = 100;
  int n_tanks   = 100;

  AttackPolicy red_policy = ATTACK_MOST_DANGEROUS;
  AttackPolicy blue_policy = ATTACK_MOST_DANGEROUS;  

  bool bounce = true;

  if (argc == 10) {
    // todo : error handling
    int i = 1;
    width = atoi(argv[i++]);
    height = atoi(argv[i++]);
    delay = atoi(argv[i++]);
    seed = atoi(argv[i++]);
    n_marines = atoi(argv[i++]);
    n_tanks = atoi(argv[i++]);
    red_policy = (AttackPolicy)atoi(argv[i++]);
    blue_policy = (AttackPolicy)atoi(argv[i++]);
    bounce = atoi(argv[i++]);
  }

  if (seed == 0) {
    // rng seed dependent on wallclock time
    unsigned long long millis =
      std::chrono::duration_cast<std::chrono::milliseconds>
      (std::chrono::system_clock::now().time_since_epoch()).count();

    seed = millis;
  }

  cout << "width: "    << width     << endl;
  cout << "height: "   << height    << endl;
  cout << "delay: "    << delay     << endl;
  cout << "seed: "     << seed      << endl;
  cout << "marines: "  << n_marines << endl;
  cout << "tanks: "    << n_tanks   << endl;
  cout << "redpol: "   << apol2str(red_policy) << endl;
  cout << "bluepol: "  << apol2str(blue_policy) << endl;
  cout << "bounce:  "  << bounce << endl;

  // set up world
  World *world = new World(width, height, seed);

  // populate with units

  // get unit sizes - a bit awkward, could be data members
  Marine *m = new Marine(RED, Vec2(0, 0), ATTACK_WEAKEST, false);
  double mr = m->radius;
  delete m;
  Tank *t = new Tank(RED, Vec2(0, 0), ATTACK_WEAKEST, false);
  double tr = t->radius;
  delete t;

  AttackPolicy red_marine_policy  = red_policy;
  AttackPolicy blue_marine_policy = blue_policy;
  AttackPolicy red_tank_policy    = red_policy;
  AttackPolicy blue_tank_policy   = blue_policy;

  // create marines
  for (int i=0; i < n_marines; ++i) {
    Unit *u = new Marine(RED, world->rnd_pos(mr), red_marine_policy, bounce);
    u->heading = world->rnd_heading();
    u->current_speed = u->max_speed;
    world->units.push_back(u);

    // mirrored    
    Unit *v = new Marine(BLUE, world->mirror(u->pos), blue_marine_policy, bounce);
    v->heading = Vec2(-u->heading.x, -u->heading.y); 
    v->current_speed = v->max_speed;
    world->units.push_back(v);
  }

  // create tanks
  for (int i=0; i < n_tanks; ++i) {
    Unit *u = new Tank(RED, world->rnd_pos(tr), red_tank_policy, bounce);
    u->heading = world->rnd_heading();
    u->current_speed = u->max_speed;
    world->units.push_back(u);

    // mirrored    
    Unit *v = new Tank(BLUE, world->mirror(u->pos), blue_tank_policy, bounce);
    v->heading = Vec2(-u->heading.x, -u->heading.y); 
    v->current_speed = v->max_speed;    
    world->units.push_back(v);
  }
   while(true){
      cout << "." << flush;
      //world->attacks.clear();
      world->step();

      int red_score = world->red_score();
      if (red_score >= 0) {
         ofstream results;
         results.open ("Results.txt", ios::app | ios::out);
         results << "game over: ";
         cout << "game over: ";
         if (red_score == 2) {
            results << "RED wins\n";
            cout << "RED wins" << endl;
         } else if (red_score == 1) {
            results << "Draw\n";
            cout << "draw" << endl;
         } else {
            results << "BLUE wins\n";
            cout << "BLUE wins" << endl;
         }
         results.close();
         delete world;
         exit(0);
      }
   }
  
  return 0;
}
