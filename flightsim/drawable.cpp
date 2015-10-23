#include "drawable.hpp"
#include "quaternion.hpp"

Drawable::Drawable() {
	next = NULL;	//link to a separate item
	shouldRemove = false;
	distanceFromCamera=-1;
}

Drawable::~Drawable() {
	delete next;	//good hygiene pointers/pointer hygiene
}

void Drawable::insert(Drawable* item){
	Drawable* iter = item;
	if (item==NULL){
		return;
	}
	while ((iter->next)!=NULL){
		iter = iter->next;
	}
	iter->next = next;
	next = item;
}

void Drawable::update(float dt){
}

Drawable* mergeSort(Drawable *start) {
    Drawable *second;

    if (start == NULL)
        return NULL;
    else if (start->next == NULL)
        return start;
    else
    {
        second = split(start);
        return merge(mergeSort(start),mergeSort(second));
    }
}

Drawable* merge(Drawable* first, Drawable* second) {
	
    if (first == NULL) return second;
    else if (second == NULL) return first;
    else if (first->distanceFromCamera >= second->distanceFromCamera) //if I reverse the sign to >=, the behavior reverses
    {
        first->next = merge(first->next, second);
        return first;
    }
    else 
    {
        second->next = merge(first, second->next);
        return second;
    }
}

Drawable* split(Drawable* start) {
    Drawable* second;

    if (start == NULL) return NULL;
    else if (start->next == NULL) return NULL;
    else {
        second = start->next;
        start->next = second->next;
        second->next = split(second->next);
        return second;
    }
}
//END

DrawableGroup::DrawableGroup(Drawable* begin):
Drawable()
{
	child = begin;
}

DrawableGroup::~DrawableGroup(){
	delete child;
}

void DrawableGroup::insert(Drawable* item){
	Drawable* iter = item;
	if (iter==NULL){
		return;
	}
	while ((iter->next)!=NULL){
		iter = iter->next;
	}
	iter->next = child;
	child = item;
}

void DrawableGroup::predraw(Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse){
	shouldDraw = false;
	Drawable *iter = child;
	int count = 0;
	while (iter != NULL){
		iter->predraw(camerapos, camerarotation, camerarotationinverse);
		shouldDraw = shouldDraw || iter->shouldDraw;
		distanceFromCamera += iter->distanceFromCamera;
		iter = iter->next;
		count++;
	}
	distanceFromCamera/=count;
	child = mergeSort(child);
	
}

void DrawableGroup::draw(sf::RenderWindow &window){
	if (shouldDraw){
		Drawable *iter = child;
		while (iter != NULL){
			iter->draw(window);
			iter = iter->next;
		}
	}
}	
