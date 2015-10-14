//very bad practice, only put in so it compiles

sf::Color getRandomColor(){
	return sf::Color(std::rand()%255,std::rand()%255,std::rand()%255);
}

Triangle* splitTriangle(Triangle* tri, float hvarbound){
	//replaces a triangle with three other triangles. the center is displaced by, at most, hvar up or down
	//works in-place on a linked list
	Drawable* next = tri->next;
	//tri->next = 0;
	Quaternion midpoint = (tri->a + tri->b + tri->c)/3;
	Quaternion var = Quaternion(0,0,hvarbound*(std::rand()%2 - 1),0);
	midpoint = midpoint + var;
	Triangle* ret = new Triangle(tri->a, tri->b, midpoint, getRandomColor());
	ret->next = 	new Triangle(tri->a, midpoint, tri->c, getRandomColor());
	ret->next->next=new Triangle(midpoint, tri->b, tri->c, getRandomColor());
	ret->next->next->next = next;
	//delete tri;
	return ret;
}

Drawable* generateTerrainBox(float x, float z, float width, float depth, int fractalIterations){
	//generates random terrain from (x to x+width) and (z to z+depth)
	Quaternion q = Quaternion(0,x,0,z);
	Triangle* head = new Triangle(Quaternion(0,0,0,0)+q, Quaternion(0,width,0,0)+q, Quaternion(0,0,0,depth)+q, getRandomColor());
	head->insert(new Triangle(Quaternion(0,width,0,depth)+q, Quaternion(0,width,0,0)+q, Quaternion(0,0,0,depth)+q, getRandomColor()));
	
	Triangle* iter;
	float var = 3;
	for (int i=0; i<fractalIterations; i++){
		head = splitTriangle(head,var);
		iter = head;
		while (iter->next->next->next!=NULL){
			iter->next->next->next = (Drawable*)splitTriangle((Triangle*)iter->next->next->next, var);
			iter = (Triangle*)iter->next->next->next;
		}
		var/=2;
	}
	
	
	return (Drawable*)head;
}
