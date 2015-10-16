#ifndef SHAPES_HPP
#define SHAPES_HPP

class Sphere: public Drawable
{
	public:
		Quaternion pos;
		sf::Color color;
		
		Sphere(Quaternion _pos, float _radius, sf::Color _color){
			pos = _pos;
			radius = _radius;
			color = _color;
			
			next = NULL;
			child = NULL;
		}
		
		void predraw(Quaternion camerapos, Quaternion camerarotation){	//
			draw_pos = camerarotation * ((pos-camerapos)* camerarotation.inverse());
			
			distanceFromCamera = draw_pos.z;
			
			render_radius = ratio * (screenwidth / 2) * radius/distanceFromCamera;
			
			shape.setFillColor(color);

			if (draw_pos.z<0)
				shape.setRadius(0);
			else
				shape.setRadius(render_radius);
				
			shape.setPosition(draw_pos.getScreenPos()+sf::Vector2f(-render_radius,-render_radius));
			//std::cout<<"radius:"<<render_radius<<std::endl;
		}
		
		void draw(sf::RenderWindow &window){
			window.draw(shape);
		}
		
	private:
		float radius;
		float render_radius;
		Quaternion draw_pos;
		sf::CircleShape shape;
};

class Line: public Drawable
{
	public:
		Quaternion start, end;
		sf::Color color;
		
		Line(Quaternion _start, Quaternion _end, sf::Color _color){
			start = _start;
			end = _end;
			color = _color;
			
			next = NULL;
			child = NULL;
		}
		
		void predraw(Quaternion camerapos, Quaternion camerarotation){	//
			drawStart = camerarotation * ((start-camerapos)* camerarotation.inverse());
			drawEnd = camerarotation * ((end-camerapos)* camerarotation.inverse());
		
		
			
			vertices[0].color = color;
			vertices[1].color = color;
			
			if ( (drawStart.z<0) and (drawEnd.z<0) ) {
				shouldDraw = false;
			}
			else {
				drawStart=clipLineToScreen(drawStart,drawEnd);
				drawEnd=clipLineToScreen(drawEnd,drawStart);
				shouldDraw = true;
			}
	
			distanceFromCamera = ((drawStart+drawEnd)/2).z;
			
			vertices[0].position = drawStart.getScreenPos();

			vertices[1].position = drawEnd.getScreenPos();

							
			//std::cout<<"radius:"<<render_radius<<std::endl;
		}
		
		void draw(sf::RenderWindow &window){
			window.draw(vertices, 2, sf::Lines);
		}
		
	private:
		sf::Vertex vertices[2];
		Quaternion drawStart, drawEnd;
	
};

class Triangle: public Drawable
{
	public:
		Quaternion a, b, c;
		sf::Color color;
		
		Triangle(Quaternion _a, Quaternion _b, Quaternion _c, sf::Color _color){
			a = _a;
			b = _b;
			c = _c;
			color = _color;
			
			next = NULL;
			child = NULL;
		}
		
		Quaternion getNormal(){
			return ((b-a).cross(c-a)).normalized();
		}
		
		void predraw(Quaternion camerapos, Quaternion camerarotation){
			a_draw = camerarotation * ((a-camerapos)* camerarotation.inverse());
			b_draw = camerarotation * ((b-camerapos)* camerarotation.inverse());
			c_draw = camerarotation * ((c-camerapos)* camerarotation.inverse());

			if ( (a_draw.z<0) and (b_draw.z<0) and (c_draw.z<0) )
				shouldDraw = false;
			else
				shouldDraw = true;
				
			distanceFromCamera = ((a_draw + b_draw + c_draw)/3).z;
			
			
			
			shape.setPointCount(6);
			shape.setPoint(0, clipLineToScreen(a_draw,b_draw).getScreenPos() );
			shape.setPoint(1, clipLineToScreen(b_draw,a_draw).getScreenPos() );
			shape.setPoint(2, clipLineToScreen(b_draw,c_draw).getScreenPos() );
			shape.setPoint(3, clipLineToScreen(c_draw,b_draw).getScreenPos() );
			shape.setPoint(4, clipLineToScreen(c_draw,a_draw).getScreenPos() );
			shape.setPoint(5, clipLineToScreen(a_draw,c_draw).getScreenPos() );
			
			int lmao = 255*(0.4+0.6*fabs(GLOBAL_LIGHT_DIRECTION.dot(getNormal())));
			shape.setFillColor(color*sf::Color(lmao, lmao, lmao));
			
		
		}
		
		void draw(sf::RenderWindow &window){
			window.draw(shape);
			//std::cout<< a_draw.getScreenPos().x << "," << a_draw.getScreenPos().y << " " << b_draw.getScreenPos().x << "," << b_draw.getScreenPos().y << " " << c_draw.getScreenPos().x << "," << c_draw.getScreenPos().y<<std::endl;
		}
		
	private:
		sf::ConvexShape shape;
		Quaternion a_draw, b_draw, c_draw;
	
};

#endif

