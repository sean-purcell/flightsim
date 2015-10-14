#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

class Drawable{
	public:
		float distanceFromCamera;
		bool shouldDraw;
		Drawable * next;
		Drawable * child;
		virtual void predraw(Quaternion camerapos, Quaternion camerarotation){};
		virtual void draw(sf::RenderWindow &window){};
		
		Drawable() {
			next = NULL;	//link to a separate item
			child = NULL;	//currently does nothing, IDEALLY this will allow us to "group" 
			distanceFromCamera=-1;
		}
		
		virtual ~Drawable() {
			delete next;	//good hygiene pointers/pointer hygiene
		}
		
		virtual void insert(Drawable* item){
			Drawable* iter = item;
			while ((iter->next)!=NULL){
				iter = iter->next;
			}
			iter->next = next;
			next = item;
		}
		
		virtual void update(float dt){
		}
		
};

//hacked together mergesort

Drawable* mergeSort(Drawable*);
Drawable* merge(Drawable*, Drawable*);
Drawable* split(Drawable*);


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


#endif

