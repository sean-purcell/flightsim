#include "drawable.hpp"
#include "windowinfo.hpp"

#include "glmheaders.hpp"

Drawable::Drawable() {
	next = NULL;	//link to a separate item
	shouldRemove = false;
	distanceFromCamera=-1;
}

Drawable::~Drawable() {
	delete next;	//good hygiene pointers/pointer hygiene
}

void Drawable::insert(Drawable* item){
	Drawable *iter = this;
	while(iter->next != NULL) {
		iter = iter->next;
	}
	iter->next = item;
}

void Drawable::predraw(vec3 camerapos, quat camerarotation) {
}

void Drawable::update(float dt){
}

void Drawable::draw(sf::RenderWindow &window) {
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

void DrawableGroup::insertInto(Drawable *item) {
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

void DrawableGroup::predraw(vec3 camerapos, quat camerarotation){
	shouldDraw = false;
	Drawable *iter = child;
	int count = 0;
	while (iter != NULL){
		iter->predraw(camerapos, camerarotation);
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
			if(iter->shouldDraw)
				iter->draw(window);
			iter = iter->next;
		}
	}
}

sf::Vector2f getScreenPos(vec3 v){
	if (v.z==0)
		return sf::Vector2f(NAN, NAN);
	return sf::Vector2f(
		ratio*(screenwidth/2.0)*v.x/v.z + (screenwidth/2.0),
		-ratio*(screenwidth/2.0)*v.y/v.z + (screenheight/2.0) );
}

vec3 clipLineToScreen(vec3 A, vec3 B){
	// if A is z-positive, returns z
	// if A is z-negative, returns a point on the line connecting A and B which is just in front of the camera
	if (A.z>0) 
		return A;
	else if(B.z > 0) {
		return A + (B-A)*((0.0001f-A.z)/(B.z-A.z));
	} else {
		return vec3(NAN, NAN, NAN);
	}
}

