#define MOVGRIDWID 10
#define MOVGRIDHEI 10
#define SCALE 1000

#include <cstdlib>

Triangle *terrain[MOVGRIDWID][MOVGRIDHEI][2];

sf::Color getColorMovTest(int x, int z, int q) {
	srand((x * 65537 + z) * (q ? 1 : -1));
	return sf::Color(rand() % 256, rand() % 256, rand() % 256);
}

Drawable *initializeTestTerrain() {
	Drawable *head = new Drawable;
	Drawable *iter = head;
	Triangle def(Quaternion(0,0,0),
		Quaternion(0,0,0),
		Quaternion(0,0,0),
		sf::Color(0,0,0));
	for(int i = 0; i < MOVGRIDWID; i++) {
		for(int j = 0; j < MOVGRIDHEI; j++) {
			terrain[i][j][0] = new Triangle(def);
			terrain[i][j][1] = new Triangle(def);
			iter->next = terrain[i][j][0];
			iter->next->next = terrain[i][j][1];
			iter = iter->next->next;
		}
	}

	iter = head->next;
	head->next = NULL;
	delete head;

	return iter;
}

void updateMovTerrain(Quaternion pos) {
	int x = ((int) pos.x) / SCALE;
	int z = ((int) pos.z) / SCALE;
	for(int i = -MOVGRIDWID/2; i < MOVGRIDWID/2; i++) {
		for(int j = -MOVGRIDHEI/2; j < MOVGRIDHEI/2; j++) {
			Triangle *iter = terrain[i + MOVGRIDWID/2][j + MOVGRIDWID/2][0];
			iter->a = Quaternion(i+x, 0, j+z) * SCALE;
			iter->b = Quaternion(i+1+x, 0, j+z) * SCALE;
			iter->c = Quaternion(i+x, 0, j+1+z) * SCALE;
			iter->color = getColorMovTest(i+x, j+z, 0);
			iter = terrain[i + MOVGRIDWID/2][j + MOVGRIDWID/2][1];
			iter->a = Quaternion(i+1+x, 0, j+1+z) * SCALE;
			iter->b = Quaternion(i+1+x, 0, j+z) * SCALE;
			iter->c = Quaternion(i+x, 0, j+1+z) * SCALE;
			iter->color = getColorMovTest(i+x, j+z, 1);
		}
	}
}

