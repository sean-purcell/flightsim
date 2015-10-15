sf::Color getRandomColor(){
	return sf::Color(std::rand()%255,std::rand()%255,std::rand()%255);
}

Triangle* splitTriangle(Triangle* tri, float hvarbound){
	//replaces a triangle with four other triangles. each of the three new edges the center is displaced by, at most, hvar up or down
	//works in-place on a linked list
	Drawable* next = tri->next;
	tri->next = 0;
	Quaternion midpoint = (tri->a + tri->b + tri->c)/3;
	Quaternion var = Quaternion(0,0,hvarbound*(std::rand()%2 - 1),0);
	midpoint = midpoint + var;
	Triangle* ret = new Triangle(tri->a, tri->b, midpoint, tri->color);
	ret->next = 	new Triangle(tri->a, midpoint, tri->c, tri->color);
	ret->next->next=new Triangle(midpoint, tri->b, tri->c, tri->color);
	ret->next->next->next = next;
	delete tri;
	return ret;
}

Drawable* generateTerrainBox(float x, float z, float width, float depth, float startY, int fractalIterations){
	//generates random terrain from (x to x+width) and (z to z+depth)
	sf::Color color = sf::Color::Green;
	Quaternion q = Quaternion(0,x,startY,z);
	Triangle* head = new Triangle(Quaternion(0,0,0,0)+q, Quaternion(0,width,0,0)+q, Quaternion(0,0,0,depth)+q, color);
	head->insert(new Triangle(Quaternion(0,width,0,depth)+q, Quaternion(0,width,0,0)+q, Quaternion(0,0,0,depth)+q, color));
	
	Triangle* iter;
	float var = 2.5;
	for (int i=0; i<fractalIterations; i++){
		head = splitTriangle(head,var);
		iter = head;
		while (iter->next->next->next!=NULL){
			iter->next->next->next = (Drawable*)splitTriangle((Triangle*)iter->next->next->next, var);
			iter = (Triangle*)iter->next->next->next;
		}
		var/=1.5;
	}
	
	
	return (Drawable*)head;
}

Drawable* iterateSplitTriangle(Triangle* tri, float variance, float varianceDecay, int fractalIterations){
	//generates random terrain from (x to x+width) and (z to z+depth)
	Triangle* iter;
	Triangle* head = tri;
	for (int i=0; i<fractalIterations; i++){
		head = splitTriangle(head,variance);
		iter = head;
		while (iter->next->next->next!=NULL){
			iter->next->next->next = (Drawable*)splitTriangle((Triangle*)iter->next->next->next, variance);
			iter = (Triangle*)iter->next->next->next;
		}
		variance*=varianceDecay;
	}
	
	
	return (Drawable*)head;
}
