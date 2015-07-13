///APPLE

Procedural::TriangleBuffer tb;
Procedural::Shape applebody = Procedural::CatmullRomSpline2()

	.addPoint(Ogre::Vector2(0,0.5))
	.addPoint(Ogre::Vector2(2,1))
	.addPoint(Ogre::Vector2(3,2))
	.addPoint(Ogre::Vector2(3.6,4))
	.addPoint(Ogre::Vector2(3,6))
	.addPoint(Ogre::Vector2(1.5,7))
	.addPoint(Ogre::Vector2(0,6.5))
	//.addPoint(Ogre::Vector2(1,8))
.realizeShape().scale(0.2).thicken(.2).getShape(0);
Procedural::Lathe().setShapeToExtrude(&applebody).addToTriangleBuffer(tb);

// stem
Procedural::Path stemPath = Procedural::CatmullRomSpline3().setNumSeg(8).addPoint(0,5,0).addPoint(1,6,0).close().realizePath();
Procedural::Shape stemShape = Procedural::CircleShape().setRadius(0.1).realizeShape();
Procedural::Extruder().setShapeToExtrude(&stemShape).setExtrusionPath(&stemPath).addToTriangleBuffer(tb);


tb.transformToMesh("apple");;

putMeshMat("apple", "redd", Vector3(13,2,13));
