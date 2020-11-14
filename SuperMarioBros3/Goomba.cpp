#include "Goomba.h"
#include "Brick.h"
#include "Mario.h"
#include "PlayScence.h"
CGoomba::CGoomba(float start_x, float final_x, int type):CEnemy( start_x, final_x,type)
{
	SetState(GOOMBA_STATE_WALKING);
	DeadTime = 0;
}

void CGoomba::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	if (this->state == GOOMBA_STATE_DIE)
			left = top = right = bottom = 0;
	else
	{
		left = x;
		top = y;
		right = x + GOOMBA_BBOX_WIDTH;

		if (state == GOOMBA_STATE_DIE)
			bottom = y + GOOMBA_BBOX_HEIGHT_DIE;
		else
			bottom = y + GOOMBA_BBOX_HEIGHT;
	}
}

void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	
	CEnemy::Update(dt, coObjects);

	//
	// TO-DO: make sure Goomba can interact with the world and to each of them too!
	// 
	vy += dt*GOOMBA_GRAVITY;
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	CalcPotentialCollisions(coObjects, coEvents);
	if (coEvents.size() == 0)
	{
			x += dx;
			y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		float x0 = x, y0 = y;
		x = x0 + dx;
		y = y0 + dy;

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (dynamic_cast<CBrick*>(e->obj))
			{
				if (nx != 0)
				{
					this->x = x0 + min_tx * this->dx + nx * 0.1f;
					this->vx = -vx;
				}
				if (ny != 0)
				{
					this->vy = 0;
					this->y = y0 + min_ty * this->dy + ny * 0.1f;
					
				}
			}
			else if (dynamic_cast<CMario*>(e->obj))
			{
				if (nx != 0)
				{
					this->x = x0 + min_tx * this->dx + nx * 0.1f;
					this->vx = -vx;
				}
				if (ny != 0)
				{
					this->vy = 0;
					this->y = y0 + min_ty * this->dy + ny * 0.1f;

				}
			}

		}

	}
	CalculateBeSwingedTail();
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CGoomba::Render()
{
	if (this->state == GOOMBA_STATE_DIE)
	{
		if (DeadTime != 0 && (GetTickCount64() - this->DeadTime) >= GOOMBA_TIME_TO_STOP_RENDERING)
			return;
	}
	int ani = GOOMBA_ANI_WALKING;
	if (state == GOOMBA_STATE_DIE)
	{
		ani = GOOMBA_ANI_DIE;
	}

	animation_set->at(ani)->Render(x, y);


	//RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
		case GOOMBA_STATE_DIE:
			y += GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE + 1;
			vx = 0;
			vy = 0;
			break;
		case GOOMBA_STATE_WALKING: 
			vx = -GOOMBA_WALKING_SPEED;
	}
}

void CGoomba::SetDeadTime()
{
	this->DeadTime = GetTickCount();
}

void CGoomba::CalculateBeSwingedTail()
{
	CMario* mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (mario->IsSwingTail)
	{
		float ml, mt, mr, mb;
		mario->GetBoundingBox(ml, mt, mr, mb);
		float gl, gt, gr, gb;
		this->GetBoundingBox(gl,gt,gr,gb);
		if (gb<mt || gt>mb)
			return;
		else
		{
			if (ml>=gr || mr <= gl)
			{
				return;
			}
			else
			{
				if (gl<ml && gr>ml)
				{
					this->SetState(GOOMBA_STATE_DIE);
				}
			}
		}
	}
}
CGoomba::~CGoomba()
{

}
