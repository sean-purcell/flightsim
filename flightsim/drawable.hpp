#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

class Drawable{
	public:
		float distanceFromCamera;
		bool shouldDraw;
		Drawable * next;
		Drawable * child;
		virtual void predraw(Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse){};
		virtual void draw(sf::RenderWindow &window){};
		
		Drawable() {
			next = NULL;	//link to a separate item
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




class DrawableGroup: public Drawable
{
	public:
		Quaternion a, b, c;
		sf::Color color;
		
		DrawableGroup(Quaternion _a, Quaternion _b, Quaternion _c, sf::Color _color){
			next = NULL;
			child = NULL;
		}
		
		void predraw(Quaternion camerapos, Quaternion camerarotation, Quaternion camerarotationinverse){
			shouldDraw = false;
			Drawable *iter = child;
			while (iter != NULL){
				iter->predraw(camerapos, camerarotation, camerarotationinverse);
				iter = iter->next;
			}
			
			child = mergeSort(child);
			
		}
		
		void draw(sf::RenderWindow &window){
			Drawable *iter = child;
			while (iter != NULL){
				iter->draw(window);
				iter = iter->next;
			}
		}
		
	private:
		sf::ConvexShape shape;
		Quaternion a_draw, b_draw, c_draw;
	
};




#endif

