///barrel
Procedural::TriangleBuffer tbb;
Procedural::Shape barrelShape = Procedural::CatmullRomSpline2()
	.addPoint(Ogre::Vector2(0,0))
	.addPoint(Ogre::Vector2(1,0))
	.addPoint(Ogre::Vector2(2,0))
	.addPoint(Ogre::Vector2(3,4))
	.addPoint(Ogre::Vector2(2,8))
	//.addPoint(Ogre::Vector2(1,8))
	.realizeShape().thicken(.2).getShape(0);

Procedural::Lathe().setShapeToExtrude(&barrelShape).addToTriangleBuffer(tbb);
//Procedural::Lathe().setShapeToExtrude(&barrelShape).addToTriangleBuffer(tb);
tbb.transformToMesh("barrel");
putMeshMat("barrel", "Examples/Rockwall", Vector3(14,1,14));
