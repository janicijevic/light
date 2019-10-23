
#include "olcPixelGameEngine.h"
#include <iostream>



struct vec2d {
	float x, y;
};
struct Rect {
	float x, y, w, h;
	bool active = true;
	olc::Pixel color;
	Rect() {}
	Rect(float _x, float _y, float _w, float _h, olc::Pixel _color = olc::WHITE) {
		x = _x;
		y = _y;
		w = _w;
		h = _h;
		color = _color;
	}

};
struct Player {
	float x, y, r;
	float dx = 0;
	float dy = 0;
	float vx = 0;
	float vy = 0;
	olc::Pixel color;
	Player() {}
	Player(float _x, float _y, float _r, olc::Pixel _color = olc::BLUE) {
		x = _x;
		y = _y;
		r = _r;
		color = _color;
	}
};

class Engine : public olc::PixelGameEngine{
public:
	Player p;
	bool pressed = false;
	bool rounding = true;
	bool wireframe = false;
	bool follow = false;
	bool absolute = false;
	bool fullCircle = false;
	int selected = 0;
	std::vector<Rect> rects;
	float maxSpeed = 150;
	float moveSpeed = maxSpeed;
	float stepAngle = 3.14 / 720;
	float w, h;
	Engine() {
		sAppName = "Demo";
	}


	bool OnUserCreate() override{
		w = ScreenWidth() / 20;
		h = ScreenHeight() / 20;

		p = Player(20, ScreenHeight()/2, 10);
		rects.push_back(Rect(w*2, h*5, w, h, olc::BLUE));
		rects.push_back(Rect(w*6, h*10, w, h, olc::BLUE));
		rects.push_back(Rect(w*3, h*16, w, h, olc::BLUE));
		rects.push_back(Rect(w*10, h*7, w, h, olc::BLUE));

		vec2d v1, v2, v3, v4;
		v1.x = 100;
		v1.y = 248;
		v2.x = 300;
		v2.y = 248;
		v3.x = 100;
		v3.y = 10;
		v4.x = 150;
		v4.y = 10;

		std::cout << isBetween(v3, v1, v2) << std::endl;

		std::cout << Intersect(v1, v2, rects[0]).x << " " << Intersect(v1, v2, rects[0]).y << std::endl;

		return true;
		
	}

	bool OnUserUpdate(float fElapsedTime) override {

		float prevX = p.x;
	 	float prevY = p.y;
		//Input
		p.dx = p.dy = 0;
		if (absolute) {
			if (GetKey(olc::Key::A).bHeld) {
				p.dx = -moveSpeed * fElapsedTime;
			}
			if (GetKey(olc::Key::D).bHeld) {
				p.dx = moveSpeed * fElapsedTime;
			}
			if (GetKey(olc::Key::W).bHeld) {
				p.dy = -moveSpeed * fElapsedTime;
			}
			if (GetKey(olc::Key::S).bHeld) {
				p.dy = moveSpeed * fElapsedTime;
			}
		}
		else {
			if (follow) {
				if (GetKey(olc::Key::W).bHeld) {
					p.dy = p.vy*moveSpeed * fElapsedTime;
					p.dx = p.vx*moveSpeed * fElapsedTime;
				}
			}
			else {
				if (GetKey(olc::Key::W).bHeld) {
					p.dy = p.vy*moveSpeed * fElapsedTime;
					p.dx = p.vx*moveSpeed * fElapsedTime;
				}
				if (GetKey(olc::Key::S).bHeld) {
					p.dy = -p.vy*moveSpeed * fElapsedTime;
					p.dx = -p.vx*moveSpeed * fElapsedTime;
				}
				if (GetKey(olc::Key::D).bHeld) {
					p.vx = p.vx*cos(stepAngle * moveSpeed * 3 * fElapsedTime) - p.vy*sin(stepAngle * moveSpeed * 3 * fElapsedTime);
					p.vy = p.vy*cos(stepAngle * moveSpeed * 3 * fElapsedTime) + p.vx*sin(stepAngle * moveSpeed * 3 * fElapsedTime);
				}
				if (GetKey(olc::Key::A).bHeld) {
					p.vx = p.vx*cos(-stepAngle * moveSpeed * 3 * fElapsedTime) - p.vy*sin(-stepAngle * moveSpeed * 3 * fElapsedTime);
					p.vy = p.vy*cos(-stepAngle * moveSpeed * 3 * fElapsedTime) + p.vx*sin(-stepAngle * moveSpeed * 3 * fElapsedTime);
				}
			}
		}
		p.x += p.dx;
		p.y += p.dy;

		//Placing blocks
		if (GetMouse(0).bPressed && !pressed) {
			pressed = true;
		}
		if (GetMouse(0).bReleased && pressed) {
			pressed = false;
			rects.push_back(Rect(round((GetMouseX()-w/2)/w)*w, round((GetMouseY()-h/2)/h)*h, w, h, olc::BLUE));
		}
		if (GetMouse(1).bPressed && pressed) {
			pressed = false;
		}
		
		//Special keys
		if (GetKey(olc::Key::E).bPressed) {
			flip(0); //wireframe
		}
		if (GetKey(olc::Key::R).bReleased) {
			flip(1); //roudning 
		}
		if (GetKey(olc::Key::F).bPressed) {
			flip(2); //follow
		}
		if (GetKey(olc::Key::Q).bPressed) {
			flip(3); //absolute
		}
		if (GetKey(olc::Key::G).bPressed) {
			flip(4); //fullCircle
		}
		if (GetKey(olc::Key::C).bPressed) {
			rects.clear();
		}
		if (GetKey(olc::Key::UP).bPressed) {
			selected = selected == 0 ? 4 : selected-1;
		}
		if (GetKey(olc::Key::DOWN).bPressed) {
			selected++;
			selected %= 5;
		}
		if (GetKey(olc::Key::ENTER).bPressed) {
			flip(selected);
		}



		//Limit
		if (p.x < 0) p.x = 0;
		if (p.x > ScreenWidth() - p.r) p.x = ScreenWidth() - p.r;
		if (p.y < 0) p.y = 0;
		if (p.y > ScreenHeight() - p.r) p.y = ScreenHeight() - p.r;

		//Collision
		p.color = olc::WHITE;
		for (int i = 0; i < rects.size(); i++) {
			float ty = p.y;
			p.y = prevY;
			if (CollidedX(p, rects[i])) {
				//p.color = olc::RED;
				p.x = prevX;
			}
			p.y = ty;
			if (CollidedY(p, rects[i])) {
				//p.color = olc::RED;
				p.y = prevY;
			}
		}

		//Get direction based on mouse
		
		if (follow) {
			vec2d mouse;
			mouse.x = GetMouseX();
			mouse.y = GetMouseY();
			p.vx = mouse.x - p.x;
			p.vy = mouse.y - p.y;
			float len = sqrt(p.vx*p.vx + p.vy*p.vy);
			p.vx = p.vx / len;
			p.vy = p.vy / len;
		}
		if (rounding) {
			float angle = atan2(p.vy, p.vx);
			angle = round(angle / stepAngle)*stepAngle;
			p.vy = sin(angle);
			p.vx = cos(angle);
		}
		//Vision
		Clear(olc::BLACK);
		float fov = 3.14/3; // deg
		if (fullCircle) fov = 2 * (3.14 + stepAngle);
		int l = p.r * 50;
		if (fullCircle) l *= 1.5;
		vec2d pp;//Previous point
		pp.x = p.x; pp.y = p.y;
		for (float i = -fov/2; i < fov/2; i+=stepAngle) {
			vec2d p1;
			p1.x = p.x+l*(p.vx*cos(i) - p.vy*sin(i));
			p1.y = p.y+l*(p.vx*sin(i) + p.vy*cos(i));
			//Draw(p1.x, p1.y);
			vec2d p2; p2.x = p.x; p2.y = p.y;
			for (int i = 0; i < rects.size(); i++) {
				vec2d v1 = Intersect(p1, p2, rects[i]);
				if (v1.x != -1 && v1.y != -1) {
					p1.x = v1.x; p1.y = v1.y;
				}
			}
			//std::cout << p1.x << " " << p1.y << std::endl;
			if (wireframe) {
				DrawLine(p.x, p.y, p1.x, p1.y);
			}
			else {
				FillTriangle(p.x, p.y, p1.x, p1.y, pp.x, pp.y, olc::CYAN);
			}
			pp.x = p1.x; pp.y = p1.y;
		}

		//Draw
		for (int i = 0; i < rects.size(); i++) {
			FillRect(rects[i].x, rects[i].y, rects[i].w, rects[i].h, rects[i].color);
		}
		FillCircle(p.x, p.y, p.r, p.color);
		FillTriangle(p.x+p.vx*p.r*2, p.y+p.vy*p.r*2, p.x-p.vy*p.r, p.y+p.vx*p.r, p.x+p.vy*p.r, p.y-p.vx*p.r, p.color);
		DrawLine(p.x, p.y, p.x+p.r*2*p.vx, p.y+p.r*2*p.vy);
		if (pressed) {
			FillRect(round((GetMouseX() - w / 2) / w)*w, round((GetMouseY() - h / 2) / h)*h, w, h, olc::BLUE);
		}
		//UI
		DrawString(5, 5,  "WireFrame (E):         " + boolText(wireframe), selected == 0 ? olc::RED : olc::WHITE);
		DrawString(5, 15, "Snap lines (R):        " + boolText(rounding), selected == 1 ? olc::RED : olc::WHITE);
		DrawString(5, 25, "Follow mouse (F):      " + boolText(follow), selected == 2 ? olc::RED : olc::WHITE);
		DrawString(5, 35, "Absolute movement (Q): " + boolText(absolute), selected == 3 ? olc::RED : olc::WHITE);
		DrawString(5, 45, "Full circle (G):       " + boolText(fullCircle), selected == 4 ? olc::RED : olc::WHITE);
		return true;
	}

	std::string boolText(bool a) {
		return a ? "On" : "Off";
	}

	void flip(int n) {
		switch (n) {
		case 0: wireframe = !wireframe; break;
		case 1: rounding = !rounding; break;
		case 2: follow = !follow; break;
		case 3: absolute = !absolute; break;
		case 4: fullCircle = !fullCircle; break;
		}
	}
	bool Collided(Player a, Rect b) {
		if (a.x - a.r < b.x + b.w && a.x + a.r > b.x && a.y - a.r < b.y + b.h && a.y + a.r > b.y) {
			return true;
		}
		return false;
	}
	bool CollidedX(Player a, Rect b) {
		if ((abs(a.x - b.x) < a.r || abs(a.x - b.x - b.w) < a.r) && a.y - a.r < b.y + b.h && a.y + a.r > b.y) {
			return true;
		}
		return false;
	}
	bool CollidedY(Player a, Rect b) {
		if ((abs(a.y - b.y) < a.r || abs(a.y - b.y - b.h) < a.r) && a.x - a.r < b.x + b.w && a.x + a.r > b.x) {
			return true;
		}
		return false;
	}

	bool isBetween(vec2d v, vec2d v1, vec2d v2) {
		////Check if a point v belongs to line segment v1v2
		float dp = (v.x - v1.x)*(v2.x - v1.x) + (v.y - v1.y)*(v2.y - v1.y);
		if (dp < 0 || dp >(v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y)) return false;
		return true;	
	}

	void Switch(float& a, float& b) {
		a = a + b;
		b = a - b;
		a = a - b;
	}
	vec2d Intersect(vec2d v1, vec2d v2, vec2d v3, vec2d v4) {
		vec2d res;
		//Formula
		float a = (v1.x - v2.x)*(v3.y - v4.y) - (v1.y - v2.y)*(v3.x - v4.x);
		res.x = ((v1.x*v2.y - v1.y*v2.x)*(v3.x - v4.x) - (v3.x*v4.y - v3.y*v4.x)*(v1.x - v2.x)) / a;
		res.y = ((v1.x*v2.y - v1.y*v2.x)*(v3.y - v4.y) - (v3.x*v4.y - v3.y*v4.x)*(v1.y - v2.y)) / a;
		//Check if intersect is valid (belongs to the line segment)
		if (!isBetween(res, v1, v2) || !isBetween(res, v3, v4)) {
			res.x = -1; res.y = -1;
		}
		if (isinf(res.x) || isnan(res.x) || isinf(res.y) || isnan(res.y)) {
			res.x = -1; res.y = -1;
		}
		return res;
	}
	vec2d Intersect(vec2d v1, vec2d v2, Rect r) {
		vec2d a, b, c, d;
		a.x = r.x; a.y = r.y;
		b.x = r.x; b.y = r.y + r.h;
		c.x = r.x + r.w; c.y = r.y + r.h;
		d.x = r.x + r.w; d.y = r.y;
		float maxDist = ScreenWidth()*ScreenHeight();
		float dist = 0;
		vec2d t = Intersect(v1, v2, a, b);
		vec2d tBest = t;
		if (t.x != -1 && t.y != -1) {
			dist = (t.x - p.x)*(t.x - p.x) + (t.y - p.y)*(t.y - p.y);
			if (dist < maxDist) {
				maxDist = dist;
				tBest = t;
			}
		}
		t = Intersect(v1, v2, b, c);
		if (t.x != -1 && t.y != -1) {
			dist = (t.x - p.x)*(t.x - p.x) + (t.y - p.y)*(t.y - p.y);
			if (dist < maxDist) {
				maxDist = dist;
				tBest = t;
			}
		}
		t = Intersect(v1, v2, c, d);
		if (t.x != -1 && t.y != -1) {
			dist = (t.x - p.x)*(t.x - p.x) + (t.y - p.y)*(t.y - p.y);
			if (dist < maxDist) {
				maxDist = dist;
				tBest = t;
			}
		}
		t = Intersect(v1, v2, a, d);
		if (t.x != -1 && t.y != -1) {
			dist = (t.x - p.x)*(t.x - p.x) + (t.y - p.y)*(t.y - p.y);
			if (dist < maxDist) {
				maxDist = dist;
				tBest = t;
			}
		}
		return tBest;
	}
};


int main(){
	Engine demo;

	if (demo.Construct(960, 960, 1, 1)) {
		demo.Start();
	}
	return 0;
}
