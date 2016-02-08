//Collision Filtering
//Kann statt Collision detection benutzt werden um verschiedene reaktionen zu erzeugen
enum collisiontypes 
    {
        COL_NOTHING  = 0,        //< Collide with nothing
        COL_WALL     = BIT(1),   //< Collide with walls
        COL_SPHERE   = BIT(2),   //< Collide with spheres
        COL_TRIANGLE = BIT(3),   //< Collide with triangles
        COL_QUAD     = BIT(4)    //< Collide with quads
    };
btRigidBody ship; // Set up the other ship stuff
btRigidBody wall; // Set up the other wall stuff
btRigidBody powerup; // Set up the other powerup stuff
int shipCollidesWith = COL_WALL;
int wallCollidesWith = COL_NOTHING;
int powerupCollidesWith = COL_SHIP | COL_WALL;
mWorld->addRigidBody(ship, COL_SHIP, shipCollidesWith);
mWorld->addRigidBody(wall, COL_WALL, wallCollidesWith);
mWorld->addRigidBody(powerup, COL_POWERUP, powerupCollidesWith);


//Collision detection Beispiel
btCollisionObject* map_cd = new btCollisionObject();

//Set the shape of each collision object
map_cd->setCollisionShape(heightmap);

//Add the collision objects to our collision world
dynamicsworldd->addCollisionObject(map_cd);


//Perform collision detection
dynamicsworld->performDiscreteCollisionDetection();
// This should be done during a simulation tick (substep) callback, because contacts might be added and removed during several substeps of a single stepSimulation call.
int numManifolds = dynamicsworld->getDispatcher()->getNumManifolds();
//For each contact manifold
for (int i = 0; i < numManifolds; i++) {
  btPersistentManifold* contactManifold = bt_collision_world->getDispatcher()->getManifoldByIndexInternal(i); 
 btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
    btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());
    contactManifold->refreshContactPoints(obA->getWorldTransform(), obB->getWorldTransform());
    int numContacts = contactManifold->getNumContacts();
    //For each contact point in that manifold
    for (int j = 0; j < numContacts; j++) {
      //Get the contact information
        btManifoldPoint& pt = contactManifold->getContactPoint(j);
        btVector3 ptA = pt.getPositionWorldOnA();
        btVector3 ptB = pt.getPositionWorldOnB();
        double ptdist = pt.getDistance();
    }
}
//Dann könnte man folgende if Bedingung aufstellen mit tags zu jedem Objekt
//int 	getIslandTag () const
 //map.setIslandTag (int tag= 2 oder so..)
//if(objA.getislandTag() == 2)
//{
//	dynamicsworld->removerigidBody...
	
//	..