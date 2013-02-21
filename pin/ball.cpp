#include "stdafx.h"

static int _balls_created;

int NumBallsInitted()
{
   return _balls_created;
}

Ball::Ball()
{
   _balls_created++;
   m_pho = NULL;
   m_pballex = NULL;
   m_vpVolObjs = NULL; // should be NULL ... only real balls have this value
   m_Event_Pos.x = m_Event_Pos.y = m_Event_Pos.z = -1.0f;
   useShadows=TRUE;
   useDecals=TRUE;
   useAntiAliasing=TRUE;
}

Ball::~Ball()	
{
   _balls_created--; //Added by JEP.  Need to keep track of number of balls on table for autostart to work.
}

void Ball::Init()
{
   // Only called by real balls, not temporary objects created for physics/rendering
   collisionMass = 1.0f;
   m_orientation.Identity();
   m_inversebodyinertiatensor.Identity((float)(5.0/2.0)/(radius*radius));
   m_angularvelocity.Set(0,0,0);
   m_angularmomentum.Set(0,0,0);

   m_ballanim.m_pball = this;

   fFrozen = false;
   // world limits on ball displacements
   //	x_min = g_pplayer->m_ptable->m_left + radius;
   //	x_max = g_pplayer->m_ptable->m_right - radius;
   //	y_min = g_pplayer->m_ptable->m_top + radius;
   //	y_max = g_pplayer->m_ptable->m_bottom - radius;
   z_min = g_pplayer->m_ptable->m_tableheight + radius;
   z_max = g_pplayer->m_ptable->m_glassheight - radius;

   m_fErase = false;

   m_pho = NULL;
   m_fDynamic = C_DYNAMIC; // assume dynamic

   m_pballex = NULL;

   m_vpVolObjs = new VectorVoid;

   m_color = RGB(255,255,255);

   if (g_pplayer->m_ptable->m_szBallImage[0] == '\0')
   {
      m_szImage[0] = '\0';
      m_pin = NULL;
   }
   else
   {
      lstrcpy(m_szImage, g_pplayer->m_ptable->m_szBallImage);
      m_pin = g_pplayer->m_ptable->GetImage(m_szImage);
   }

   if (g_pplayer->m_ptable->m_szBallImageFront[0] == '\0')
   {
      m_szImageFront[0] = '\0';
      m_pinFront = NULL;
   }
   else
   {
      lstrcpy(m_szImageFront, g_pplayer->m_ptable->m_szBallImageFront);
      m_pinFront = g_pplayer->m_ptable->GetImage(m_szImageFront);
   }

   if (g_pplayer->m_ptable->m_szBallImageBack[0] == '\0')
   {
      m_szImageBack[0] = '\0';
      m_pinBack = NULL;
   }
   else
   {
      lstrcpy(m_szImageBack, g_pplayer->m_ptable->m_szBallImageBack);
      m_pinBack = g_pplayer->m_ptable->GetImage(m_szImageBack);
   }

   mtrl.specular.r = mtrl.specular.g =	mtrl.specular.b = mtrl.specular.a =
      mtrl.emissive.r = mtrl.emissive.g =	mtrl.emissive.b = mtrl.emissive.a =
      mtrl.power = 0;
   mtrl.diffuse.a = mtrl.ambient.a = 1.0f;
   rgv3D[0].tu = 0;
   rgv3D[0].tv = 0;
   rgv3D[0].nx = 0;
   rgv3D[0].ny = 0;
   rgv3D[0].nz = -1.0f;

   rgv3D[3].tu = 0;
   rgv3D[3].nx = 0;
   rgv3D[3].ny = 0;
   rgv3D[3].nz = -1.0f;

   rgv3D[2].nx = 0;
   rgv3D[2].ny = 0;
   rgv3D[2].nz = -1.0f;

   rgv3D[1].tv = 0;
   rgv3D[1].nx = 0;
   rgv3D[1].ny = 0;
   rgv3D[1].nz = -1.0f;

   rgv3DArrow[0].tu = 0;
   rgv3DArrow[0].tv = 0;
   rgv3DArrow[0].x = -0.333333333f;
   rgv3DArrow[0].y = -0.333333333f;
   rgv3DArrow[0].z = -0.881917103f;

   rgv3DArrow[1].tu = 1.0f;
   rgv3DArrow[1].tv = 0;
   rgv3DArrow[1].x = 0.333333333f;
   rgv3DArrow[1].y = -0.333333333f;
   rgv3DArrow[1].z = -0.881917103f;

   rgv3DArrow[2].tu = 1.0f;
   rgv3DArrow[2].tv = 1.0f;
   rgv3DArrow[2].x = 0.333333333f;
   rgv3DArrow[2].y = 0.333333333f;
   rgv3DArrow[2].z = -0.881917103f;

   rgv3DArrow[3].tu = 0;
   rgv3DArrow[3].tv = 1.0f;
   rgv3DArrow[3].x = -0.333333333f;
   rgv3DArrow[3].y = 0.333333333f;
   rgv3DArrow[3].z = -0.881917103f;

   m_rgv3DShadow[0].tu = 0;
   m_rgv3DShadow[0].tv = 0;
   m_rgv3DShadow[0].nx = 0;
   m_rgv3DShadow[0].ny = 0;
   m_rgv3DShadow[0].nz = -1.0f;

   m_rgv3DShadow[1].tu = 1.0f;
   m_rgv3DShadow[1].tv = 0;
   m_rgv3DShadow[1].nx = 0;
   m_rgv3DShadow[1].ny = 0;
   m_rgv3DShadow[1].nz = -1.0f;

   m_rgv3DShadow[2].tu = 1.0f;
   m_rgv3DShadow[2].tv = 1.0f;
   m_rgv3DShadow[2].nx = 0;
   m_rgv3DShadow[2].ny = 0;
   m_rgv3DShadow[2].nz = -1.0f;

   m_rgv3DShadow[3].tu = 0;
   m_rgv3DShadow[3].tv = 1.0f;
   m_rgv3DShadow[3].nx = 0;
   m_rgv3DShadow[3].ny = 0;
   m_rgv3DShadow[3].nz = -1.0f;


}


void Ball::EnsureOMObject()
{
   if (m_pballex)
   {
      return;
   }

   CComObject<BallEx>::CreateInstance(&m_pballex);
   m_pballex->AddRef();

   m_pballex->m_pball = this;
}

void Ball::CalcBoundingRect()
{
   const float dx = fabsf(vx);
   const float dy = fabsf(vy);

   m_rcHitRect.left   = x - (radius + 0.1f + dx); //!! make more accurate ????
   m_rcHitRect.right  = x + (radius + 0.1f + dx);
   m_rcHitRect.top    = y - (radius + 0.1f + dy);	
   m_rcHitRect.bottom = y + (radius + 0.1f + dy);

   m_rcHitRect.zlow  = min(z, z+vz) - radius;
   m_rcHitRect.zhigh = max(z, z+vz) + (radius + 0.1f);
}

void Ball::CollideWall(const Vertex3Ds * const phitnormal, const float m_elasticity, float antifriction, float scatter_angle)
{
   float dot = vx * phitnormal->x + vy * phitnormal->y; //speed normal to wall

   if (dot >= -C_LOWNORMVEL )							// nearly receding ... make sure of conditions
   {												// otherwise if clearly approaching .. process the collision
      if (dot > C_LOWNORMVEL) return;					//is this velocity clearly receding (i.e must > a minimum)		
#ifdef C_EMBEDDED
      if (m_HitDist < -C_EMBEDDED)
         dot = -C_EMBEDSHOT;							// has ball become embedded???, give it a kick
      else return;
#endif
   } 

#ifdef C_DISP_GAIN 
   float hdist = -C_DISP_GAIN * m_HitDist;			// limit delta noise crossing ramps,
   if (hdist > 1.0e-4f)
   {
      if (hdist > C_DISP_LIMIT) 
         hdist = C_DISP_LIMIT;	// crossing ramps, delta noise
      x += hdist * phitnormal->x;	// push along norm, back to free area
      y += hdist * phitnormal->y;	// use the norm, but this is not correct, reverse time is correct
   }
#endif		

   dot *= -1.005f - m_elasticity; //!! some small minimum
   vx += dot * phitnormal->x;	
   vy += dot * phitnormal->y;

   if (antifriction >= 1.0f || antifriction <= 0.0f) 
      antifriction = c_hardFriction; // use global

   vx *= antifriction; vy *= antifriction; vz *= antifriction;			//friction all axiz

   if (scatter_angle <= 0.0f) scatter_angle = c_hardScatter;			// if <= 0 use global value
   scatter_angle *= g_pplayer->m_ptable->m_globalDifficulty;			// apply dificulty weighting

   if (dot > 1.0f && scatter_angle > 1.0e-5f) //no scatter at low velocity 
   {
      float scatter = rand_mt_m11();									// -1.0f..1.0f
      scatter *= (1.0f - scatter*scatter)*2.59808f * scatter_angle;	// shape quadratic distribution and scale
      const float radsin = sinf(scatter); // Green's transform matrix... rotate angle delta 
      const float radcos = cosf(scatter); // rotational transform from current position to position at time t
      const float vxt = vx; 
      const float vyt = vy;
      vx = vxt *radcos - vyt *radsin;  // rotate to random scatter angle
      vy = vyt *radcos + vxt *radsin; 
   }

   const Vertex3Ds vnormal(phitnormal->x, phitnormal->y, 0.0f); //??? contact point
   AngularAcceleration(&vnormal);	//calc new rolling dynamics
}


void Ball::Collide3DWall(const Vertex3Ds * const phitnormal, const float m_elasticity, float antifriction, float scatter_angle)
{
   float dot = vx * phitnormal->x + vy * phitnormal->y + vz * phitnormal->z; //speed normal to wall

   if (dot >= -C_LOWNORMVEL )								// nearly receding ... make sure of conditions
   {													// otherwise if clearly approaching .. process the collision
      if (dot > C_LOWNORMVEL) return;						//is this velocity clearly receding (i.e must > a minimum)	

#ifdef C_EMBEDDED
      if (m_HitDist < -C_EMBEDDED)
         dot = -C_EMBEDSHOT;		// has ball become embedded???, give it a kick
      else return;
#endif
   }

#ifdef C_DISP_GAIN 		
   // correct displacements, mostly from low velocity, alternative to acceleration processing
   float hdist = -C_DISP_GAIN * m_HitDist;			// limit delta noise crossing ramps, 
   if (hdist > 1.0e-4f)					// when hit detection checked it what was the displacement
   {			
      if (hdist > C_DISP_LIMIT) 
         hdist = C_DISP_LIMIT;	// crossing ramps, delta noise			
      x += hdist * phitnormal->x;	// push along norm, back to free area
      y += hdist * phitnormal->y;	// use the norm, but his is not correct
      z += hdist * phitnormal->z;	// 
   }	
#endif					

   if (antifriction >= 1.0f || antifriction <= 0.0f) 
      antifriction = c_hardFriction; // use global

   dot *= -1.005f - m_elasticity;	
   vx += dot * phitnormal->x;
   vx *= antifriction;
   vy += dot * phitnormal->y;
   vy *= antifriction;
   vz += dot * phitnormal->z;
   vz *= antifriction;

   if (scatter_angle <= 0.0f) scatter_angle = c_hardScatter;			// if <= zero use global value
   scatter_angle *= g_pplayer->m_ptable->m_globalDifficulty;			// apply dificulty weighting

   if (dot > 1.0f && scatter_angle > 1.0e-5f) //no scatter at low velocity 
   {
      float scatter = rand_mt_m11();								    // -1.0f..1.0f
      scatter *= (1.0f - scatter*scatter)*2.59808f * scatter_angle;	// shape quadratic distribution and scale
      const float radsin = sinf(scatter); // Green's transform matrix... rotate angle delta
      const float radcos = cosf(scatter); // rotational transform from current position to position at time t
      const float vxt = vx; 
      const float vyt = vy;
      vx = vxt *radcos - vyt *radsin;  // rotate to random scatter angle
      vy = vyt *radcos + vxt *radsin;
   }

   AngularAcceleration(phitnormal);	 // calc new rolling dynmaics
}


float Ball::HitTest(Ball * const pball, const float dtime, Vertex3Ds * const phitnormal)
{	
   const float dvx = vx - vx;		// delta velocity 
   const float dvy = vy - vy;
   float dvz = vz - vz;

   const float dx = x - x;			// delta position
   const float dy = y - y;
   float dz = z - z;

   float bcddsq = dx*dx + dy*dy + dz*dz;	//square of ball center's delta distance

   float bcdd = sqrtf(bcddsq);				// length of delta
   if (bcdd < 1.0e-8f)						// two balls center-over-center embedded
   { //return -1;
      dz = -1.0f;							// patch up			
      z -= dz;						//lift up

      bcdd = 1.0f;						//patch up
      bcddsq = 1.0f;						//patch up
      dvz = 0.1f;							// small speed difference
      vz -= dvz;
   }

   float b = dvx*dx + dvy*dy + dvz*dz;		// inner product
   const float bnv = b/bcdd;				// normal speed of balls toward each other

   if ( bnv > C_LOWNORMVEL) return -1.0f;	// dot of delta velocity and delta displacement, postive if receding no collison

   const float totalradius = radius + radius;
   const float bnd = bcdd - totalradius;

   float hittime;
   if (bnd < (float)PHYS_TOUCH)			// in contact??? 
   {
      if (bnd <= (float)(-PHYS_SKIN*2.0))
         return -1.0f;					// embedded too deep

      if ((fabsf(bnv) > C_CONTACTVEL)			// >fast velocity, return zero time
         //zero time for rigid fast bodies
         || (bnd <= (float)(-PHYS_TOUCH)))
         hittime = 0;						// slow moving but embedded
      else
         hittime = bnd*(float)(1.0/(2.0*PHYS_TOUCH)) + 0.5f;		// don't compete for fast zero time events
   }
   else
   {
      const float a = dvx*dvx + dvy*dvy + dvz*dvz;				//square of differential velocity 

      if (a < 1.0e-8f) return -1.0f;				// ball moving really slow, then wait for contact

      const float c = bcddsq - totalradius*totalradius;	//first contact test: square delta position - square of radii
      b += b;										// two inner products
      float result = b*b - 4.0f*a*c;				// squareroot term in Quadratic equation

      if (result < 0.0f) return -1.0f;			// no collision path exist	

      result = sqrtf(result);

      const float inv_a = (-0.5f)/a;
      float time1 = (b - result)*inv_a;
      const float time2 = (b + result)*inv_a;

      if (time1 < 0) time1 = time2;				// if time1 negative, assume time2 postive

      hittime = (time1 < time2) ? time1 : time2;	// select lessor
      // if time2 is negative ... 

      if (infNaN(hittime) || hittime < 0 || hittime > dtime) return -1.0f; // .. was some time previous || beyond the next physics tick
   }

   const float hitx = x + dvx * hittime;  // new ball position
   const float hity = y + dvy * hittime;
   const float hitz = z + dvz * hittime;

   const float len = (hitx - x)*(hitx - x)+(hity - y)*(hity - y)+(hitz - z)*(hitz - z);
   const float inv_len = (len > 0.0f) ? 1.0f/sqrtf(len) : 0.0f;

   phitnormal->x = (hitx - x)*inv_len;	//calc unit normal of collision
   phitnormal->y = (hity - y)*inv_len;
   phitnormal->z = (hitz - z)*inv_len;

   m_HitDist = bnd;					//actual contact distance 
   m_HitNormVel = bnv;
   m_HitRigid = true;					//rigid collision type

   return hittime;	
}


void Ball::Collide(Ball * const pball, Vertex3Ds * const phitnormal)
{
   if (fFrozen) 
      return;

   const Vertex3Ds vnormal = *phitnormal;

   // correct displacements, mostly from low velocity, alternative to true acceleration processing

   const Vertex3Ds vel(
      vx * collisionMass -vx * collisionMass,  //target ball to object ball
      vy * collisionMass -vy * collisionMass,  //delta velocity
      vz * collisionMass -vz * collisionMass);

   float dot = vel.x * vnormal.x + vel.y * vnormal.y + vel.z * vnormal.z;

   if (dot >= -C_LOWNORMVEL )								// nearly receding ... make sure of conditions
   {													// otherwise if clearly approaching .. process the collision
      if (dot > C_LOWNORMVEL) return;						//is this velocity clearly receding (i.e must > a minimum)		
#ifdef C_EMBEDDED
      if (m_HitDist < -C_EMBEDDED)
         dot = -C_EMBEDSHOT;		// has ball become embedded???, give it a kick
      else return;
#endif
   }

#ifdef C_DISP_GAIN 		
   float edist = -C_DISP_GAIN * m_HitDist; // 
   if (edist > 1.0e-4f)
   {										
      if (edist > C_DISP_LIMIT) 
         edist = C_DISP_LIMIT;		// crossing ramps, delta noise
      if (!fFrozen) edist *= 0.5f;	// if the hitten ball is not frozen
      x += edist * vnormal.x;	// push along norm, back to free area
      y += edist * vnormal.y;	// use the norm, but is not correct, but cheaply handled
      z += edist * vnormal.z;	// 
   }

   edist = -C_DISP_GAIN * m_HitDist;	// noisy value .... needs investigation
   if (!fFrozen && edist > 1.0e-4f)
   { 
      if (edist > C_DISP_LIMIT) 
         edist = C_DISP_LIMIT;		// crossing ramps, delta noise
      edist *= 0.5f;		
      x -= edist * vnormal.x;			// pull along norm, back to free area
      y -= edist * vnormal.y;			// use the norm
      z -= edist * vnormal.z;			//
   }
#endif				

   const float averageMass = (collisionMass + collisionMass)*0.5f;
   const float impulse1 = ((float)(-1.8 * 0.5) * dot) * collisionMass / (averageMass * collisionMass);
   float impulse2 = ((float)(-1.8 * 0.5) * dot) * collisionMass / (averageMass * collisionMass);

   if (!fFrozen)
   {
      vx -= impulse1 * vnormal.x;
      vy -= impulse1 * vnormal.y;
      vz -= impulse1 * vnormal.z;
      m_fDynamic = C_DYNAMIC;		
   }
   else impulse2 += impulse1;

   vx += impulse2 * vnormal.x;
   vy += impulse2 * vnormal.y;
   vz += impulse2 * vnormal.z;
   m_fDynamic = C_DYNAMIC;
}

void Ball::AngularAcceleration(const Vertex3Ds * const phitnormal)
{
   const Vertex3Ds bccpd(
      -radius * phitnormal->x,
      -radius * phitnormal->y,
      -radius * phitnormal->z);				// vector ball center to contact point displacement

   const float bnv = vx * phitnormal->x + vy * phitnormal->y + vz * phitnormal->z; //ball normal velocity to hit face

   const Vertex3Ds bvn(
      bnv * phitnormal->x,					//project the normal velocity along normal
      bnv * phitnormal->y,
      bnv * phitnormal->z);

   const Vertex3Ds bvt(
      vx - bvn.x,								// calc the tangent velocity
      vy - bvn.y,
      vz - bvn.z);

   Vertex3Ds bvT(bvt.x, bvt.y, bvt.z);			// ball tangent velocity Unit Tangent
   bvT.Normalize();	

   const Vertex3Ds bstv =						// ball surface tangential velocity
      CrossProduct(m_angularvelocity, bccpd);		// velocity of ball surface at contact point

   const float dot = bstv.Dot(bvT);			// speed ball surface contact point tangential to contact surface point
   const Vertex3Ds cpvt(						// contact point velocity tangential to hit face
      bvT.x * dot,
      bvT.y * dot,
      bvT.z * dot);

   const Vertex3Ds slideVel(					// contact point slide velocity with ball center velocity
      bstv.x - cpvt.x,						// slide velocity
      bstv.y - cpvt.y,
      bstv.z - cpvt.z);

   m_angularmomentum.MultiplyScalar(0.99f);

   // If the point and the ball are travelling in opposite directions,
   // and the point's velocity is at least the magnitude of the balls,
   // then we have a natural rool

   Vertex3Ds cpctrv(
      -slideVel.x,							//contact point co-tangential reverse velocity
      -slideVel.y,
      -slideVel.z);

   // Calculate the maximum amount the point velocity can change this
   // time segment due to friction
   Vertex3Ds FrictionForce(
      cpvt.x + bvt.x,
      cpvt.y + bvt.y,
      cpvt.z + bvt.z);

   // If the point can change fast enough to go directly to a natural roll, then do it.

   if (FrictionForce.LengthSquared() > (float)(ANGULARFORCE*ANGULARFORCE))
   {
      FrictionForce.Normalize(ANGULARFORCE);
   }

   if (vx*vx + vy*vy + vz*vz > (float)(0.7*0.7))
   {
      cpctrv.x -= FrictionForce.x;
      cpctrv.y -= FrictionForce.y;
      cpctrv.z -= FrictionForce.z;
   }

   // Divide by the inertial tensor for a sphere in order to change
   // linear force into angular momentum
   cpctrv.x *= (float)(1.0/2.5); // Inertial tensor for a sphere
   cpctrv.y *= (float)(1.0/2.5);
   cpctrv.z *= (float)(1.0/2.5);

   const Vertex3Ds vResult = CrossProduct(bccpd, cpctrv); // ball center contact point displacement X reverse contact point co-tan vel

   m_angularmomentum.Add(vResult); // add delta 

   m_angularvelocity = m_inverseworldinertiatensor.MultiplyVector(m_angularmomentum);
}

void Ball::CalcHitRect()
{
}

void BallAnimObject::UpdateDisplacements(const float dtime)
{
   m_pball->UpdateDisplacements(dtime);
}

void Ball::UpdateDisplacements(const float dtime)
{    	
   if (!fFrozen)
   {
      const float dsx = vx * dtime;
      const float dsy = vy * dtime;
      const float dsz = vz * dtime;
      x += dsx;
      y += dsy;
      z += dsz;

      drsq = dsx*dsx + dsy*dsy + dsz*dsz;				// used to determine if static ball

      if (vz < 0.f && z <= z_min)						//rolling point below the table and velocity driving deeper
      {
         z = z_min;									// set rolling point to table surface
         vz *= -0.2f;							    // reflect velocity  ...  dull bounce

         vx *= c_hardFriction;
         vy *= c_hardFriction;						//friction other axiz

         const Vertex3Ds vnormal(0.0f,0.0f,1.0f);
         AngularAcceleration(&vnormal);
      }
      else if (vz > 0.f && z >= z_max)				//top glass ...contact and going higher
      {
         z = z_max;									// set diametric rolling point to top glass
         vz *= -0.2f;								// reflect velocity  ...  dull bounce
      }

      /*		if (vx < 0.f && x <= x_min)						//left wall
      {
      x = x_min;									
      vx *= -0.2f;
      }
      else if (vx > 0.f && x >= x_max)				//right wall
      {
      x = x_max;							
      vx *= -0.2f;
      }

      if (vy < 0.f && y <= y_min)						//top wall
      {
      y = y_min;									
      vy *= -0.2f;
      }
      else if (vy > 0.f && y >= y_max)				//bottom wall
      {
      y = y_max;							
      vy *= -0.2f;
      }
      */
      CalcBoundingRect();

      Matrix3 mat3;
      mat3.CreateSkewSymmetric(m_angularvelocity);

      Matrix3 addedorientation;
      addedorientation.MultiplyMatrix(&mat3, &m_orientation);

      addedorientation.MultiplyScalar(dtime);

      m_orientation.AddMatrix(&addedorientation, &m_orientation);

      m_orientation.OrthoNormalize();

      Matrix3 matTransposeOrientation;
      m_orientation.Transpose(&matTransposeOrientation);
      m_inverseworldinertiatensor.MultiplyMatrix(&m_orientation,&m_inversebodyinertiatensor);
      m_inverseworldinertiatensor.MultiplyMatrix(&m_inverseworldinertiatensor,&matTransposeOrientation);

      m_angularvelocity = m_inverseworldinertiatensor.MultiplyVector(m_angularmomentum);
   }
}

void BallAnimObject::UpdateVelocities()
{
   m_pball->UpdateVelocities();
}

void Ball::UpdateVelocities()
{
   const float g = g_pplayer->m_gravity.z;
   float nx = g_pplayer->m_NudgeX;
   float ny = g_pplayer->m_NudgeY;

   if(g_pplayer->m_NudgeManual >= 0) //joystick control of ball roll
   {
      vx *= 0.92f;	//rolling losses high for easy manual control
      vy *= 0.92f;
      vz *= 0.92f;	

#define JOY_DEADBAND  5.0e-2f

      const float mag = nx*nx + ny*ny;// + nz*nz;
      if (mag > (JOY_DEADBAND * JOY_DEADBAND))			//joystick dead band, allows hold and very slow motion
      {
         const float inv = (mag > 0.0f) ? JOY_DEADBAND/sqrtf(mag) : 0.0f;
         nx -= nx*inv;	// remove deadband offsets
         ny -= ny*inv; 
         //nz -= nz*inv;

         vx += nx;
         vy += ny;
         vz += g;//-c_Gravity;
      }
   }//manual joystick control
   else if (!fFrozen)  // Gravity	
   {
      vx += g_pplayer->m_gravity.x;	
      vy += g_pplayer->m_gravity.y;	

      if (z > z_min + 0.05f || g > 0) // off the deck??? or gravity postive Z direction	
         vz += g;
      else vz += g * 0.001f;			// don't add so much energy if already on the world floor

      vx += nx;
      vy += ny;
   }

   const float mag = vx*vx + vy*vy + vz*vz; //speed check 
   const float antifrict = (mag > c_maxBallSpeedSqr) ? c_dampingFriction : 0.99875f;

   vx *= antifrict;	// speed damping
   vy *= antifrict;
   vz *= antifrict;	

   m_fDynamic = C_DYNAMIC;		// always set .. after adding velocity

   CalcBoundingRect();
}

void Ball::InvalidateRect(RECT * const prc)
{
   // This assumes the caller does not need *prc any more!!!
   // Either that, or we assume it can be permanently changed,
   // Because we never care about redrawing stuff off the screen.
   if (prc->top < 0)
   {
      prc->top = 0;
   }

   UpdateRect * const pur = new UpdateRect();
   pur->m_rcupdate = *prc;
   pur->m_fSeeThrough = fTrue;

   // Check all animated objects.
   for (int i=0;i<g_pplayer->m_vscreenupdate.Size();++i)
   {
      // Get the bounds of this animated object.
      const RECT * const prc2 = &g_pplayer->m_vscreenupdate.ElementAt(i)->m_rcBounds;

      // Check if the bounds of the animated object are within the bounds of our invalid rectangle.
      if ((prc->right >= prc2->left) && (prc->left <= prc2->right) && (prc->bottom >= prc2->top) && (prc->top <= prc2->bottom))
      {
         // Add to this rect's list of objects that need to be redrawn.
         pur->m_vobject.AddElement(g_pplayer->m_vscreenupdate.ElementAt(i));
      }
   }

   // Add the rect.
   g_pplayer->m_vupdaterect.AddElement(pur);
}

void Ball::Draw()
{
   float radiusX = radius * g_pplayer->GetBallStretchX();
   float radiusY = radius * g_pplayer->GetBallStretchY();

   const float sn = sinf(g_pplayer->m_pin3d.m_inclination);
   const float cs = cosf(g_pplayer->m_pin3d.m_inclination);

   const float r = (m_color & 255) * (float)(1.0/255.0);
   const float g = (m_color & 65280) * (float)(1.0/65280.0);
   const float b = (m_color & 16711680) * (float)(1.0/16711680.0);
   mtrl.diffuse.r = mtrl.ambient.r = r;
   mtrl.diffuse.g = mtrl.ambient.g = g;
   mtrl.diffuse.b = mtrl.ambient.b = b;

   const float zheight = (!fFrozen) ? z : (z - radius);

   rgv3D[0].x = x - radiusX;
   rgv3D[0].y = y - (radiusY * cs);
   rgv3D[0].z = zheight + (radius * sn);
   rgv3D[0].tu = 0;
   rgv3D[0].tv = 0;
   rgv3D[0].nx = 0;
   rgv3D[0].ny = 0;
   rgv3D[0].nz = -1.0f;

   rgv3D[3].x = x - radiusX;
   rgv3D[3].y = y + (radiusY * cs);
   rgv3D[3].z = zheight - (radius * sn);

   rgv3D[2].x = x + radiusX;
   rgv3D[2].y = y + (radiusY * cs);
   rgv3D[2].z = zheight - (radius * sn);

   rgv3D[1].x = x + radiusX;
   rgv3D[1].y = y - (radiusY * cs);
   rgv3D[1].z = zheight + (radius * sn);

   g_pplayer->m_pin3d.m_pd3dDevice->SetRenderState(RenderDevice::ALPHAREF, (DWORD)0x0000001);
   g_pplayer->m_pin3d.m_pd3dDevice->SetRenderState(RenderDevice::ALPHAFUNC, D3DCMP_GREATEREQUAL);
   g_pplayer->m_pin3d.m_pd3dDevice->SetRenderState(RenderDevice::ALPHATESTENABLE, TRUE); 

   if ( useAntiAliasing )
   {
      g_pplayer->m_pin3d.m_pd3dDevice->SetRenderState(RenderDevice::COLORKEYENABLE, FALSE);
      g_pplayer->m_pin3d.m_pd3dDevice->SetRenderState(RenderDevice::ALPHABLENDENABLE, TRUE);
      if( useShadows )
      {
         g_pplayer->m_pin3d.m_pd3dDevice->SetRenderState(RenderDevice::ZWRITEENABLE, FALSE);
         DrawShadow();
      }
      g_pplayer->m_pin3d.m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTFP_LINEAR);
   }
   else
   {
      g_pplayer->m_pin3d.m_pd3dDevice->SetRenderState(RenderDevice::COLORKEYENABLE, TRUE);
      g_pplayer->m_pin3d.m_pd3dDevice->SetRenderState(RenderDevice::ALPHABLENDENABLE, FALSE);
      g_pplayer->m_pin3d.m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTFP_NONE);
   }

   if (!m_pin)
   {
      g_pplayer->m_pin3d.m_pd3dDevice->SetTexture(0, g_pplayer->m_pin3d.m_pddsBallTexture);
      rgv3D[3].tv = 1.0f;
      rgv3D[2].tu = 1.0f;
      rgv3D[2].tv = 1.0f;
      rgv3D[1].tu = 1.0f;
   }
   else
   {
      m_pin->EnsureColorKey();
      g_pplayer->m_pin3d.m_pd3dDevice->SetTexture(0, m_pin->m_pdsBufferColorKey);
      rgv3D[3].tv = m_pin->m_maxtv;
      rgv3D[2].tu = m_pin->m_maxtu;
      rgv3D[2].tv = m_pin->m_maxtv;
      rgv3D[1].tu = m_pin->m_maxtu;
   }

   g_pplayer->m_pin3d.m_pd3dDevice->setMaterial(&mtrl);
//   g_pplayer->m_pin3d.m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
   g_pplayer->m_pin3d.m_pd3dDevice->SetRenderState(RenderDevice::ZWRITEENABLE, TRUE);
   g_pplayer->m_pin3d.m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
   //m_pin3d.m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
   g_pplayer->m_pin3d.m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
   g_pplayer->m_pin3d.m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR);

   g_pplayer->m_pin3d.m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, MY_D3DFVF_NOTEX2_VERTEX, rgv3D, 4,  (LPWORD)rgi0123, 4, NULL);

   Vertex3D_NoTex2 rgv3DArrowTransformed[4];
   Vertex3D_NoTex2 rgv3DArrowTransformed2[4];
   if (useDecals && (m_pinFront || m_pinBack))
   {
      mtrl.diffuse.a = mtrl.ambient.a = 0.8f;//0.7f;
      g_pplayer->m_pin3d.m_pd3dDevice->setMaterial(&mtrl);

      //Vertex3D_NoTex2 rgv3DArrow[4];

      g_pplayer->m_pin3d.m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
      g_pplayer->m_pin3d.m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
      g_pplayer->m_pin3d.m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
      g_pplayer->m_pin3d.m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTFP_LINEAR);

      g_pplayer->m_pin3d.m_pd3dDevice->SetRenderState(RenderDevice::COLORKEYENABLE, FALSE);
      g_pplayer->m_pin3d.m_pd3dDevice->SetRenderState(RenderDevice::ALPHABLENDENABLE, TRUE);

      g_pplayer->m_pin3d.m_pd3dDevice->SetRenderState(RenderDevice::ZWRITEENABLE, FALSE);

      // Scale the orientation for Ball stretching
      Matrix3 orientation;
      orientation.Identity();
      orientation.scaleX(g_pplayer->GetBallStretchX());
      orientation.scaleY(g_pplayer->GetBallStretchY());
      orientation.MultiplyMatrix(&orientation, &m_orientation);
      if (m_pinFront)
      {
         m_pinFront->EnsureColorKey();
         g_pplayer->m_pin3d.m_pd3dDevice->SetTexture(0, m_pinFront->m_pdsBufferColorKey);

         for (int iPoint=0;iPoint<4;iPoint++)
         {
            const Vertex3Ds tmp = orientation.MultiplyVector(rgv3DArrow[iPoint]);
            rgv3DArrowTransformed[iPoint].nx = tmp.x;
            rgv3DArrowTransformed[iPoint].ny = tmp.y;
            rgv3DArrowTransformed[iPoint].nz = tmp.z;
            rgv3DArrowTransformed[iPoint].x = x - tmp.x*radius;
            rgv3DArrowTransformed[iPoint].y = y - tmp.y*radius;
            rgv3DArrowTransformed[iPoint].z = zheight  - tmp.z*radius;
            rgv3DArrowTransformed[iPoint].tu = rgv3DArrow[iPoint].tu * m_pinFront->m_maxtu;
            rgv3DArrowTransformed[iPoint].tv = rgv3DArrow[iPoint].tv * m_pinFront->m_maxtv;
         }
         g_pplayer->m_pin3d.m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, MY_D3DFVF_NOTEX2_VERTEX, rgv3DArrowTransformed, 4, (LPWORD)rgi0123, 4, NULL);
      }
      orientation.Identity();
      orientation.scaleX(g_pplayer->GetBallStretchX());
      orientation.scaleY(g_pplayer->GetBallStretchY());
      orientation.MultiplyMatrix(&orientation, &m_orientation);
      if (m_pinBack)
      {
         // Other side of ball
         m_pinBack->EnsureColorKey();
         g_pplayer->m_pin3d.m_pd3dDevice->SetTexture(0, m_pinBack->m_pdsBufferColorKey);

         for (int iPoint=0;iPoint<4;iPoint++)
         {
            rgv3DArrow[iPoint].x = -rgv3DArrow[iPoint].x;
            rgv3DArrow[iPoint].z = -rgv3DArrow[iPoint].z;
            const Vertex3Ds tmp = orientation.MultiplyVector(rgv3DArrow[iPoint]);
            rgv3DArrowTransformed2[iPoint].nx = tmp.x;
            rgv3DArrowTransformed2[iPoint].ny = tmp.y;
            rgv3DArrowTransformed2[iPoint].nz = tmp.z;
            rgv3DArrowTransformed2[iPoint].x = x - tmp.x*radius;
            rgv3DArrowTransformed2[iPoint].y = y - tmp.y*radius;
            rgv3DArrowTransformed2[iPoint].z = zheight  - tmp.z*radius;
            rgv3DArrowTransformed2[iPoint].tu = rgv3DArrow[iPoint].tu * m_pinBack->m_maxtu;
            rgv3DArrowTransformed2[iPoint].tv = rgv3DArrow[iPoint].tv * m_pinBack->m_maxtv;
         }

         g_pplayer->m_pin3d.m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, MY_D3DFVF_NOTEX2_VERTEX, rgv3DArrowTransformed2, 4, (LPWORD)rgi0123, 4, NULL);
      }
   }

   m_fErase = true;

   // Mark ball rect as dirty for blitting to the screen
   g_pplayer->m_pin3d.ClearExtents(&m_rcScreen, NULL, NULL);
   g_pplayer->m_pin3d.ExpandExtentsPlus(&m_rcScreen, rgv3D, NULL, NULL, 4, fFalse);

   if (useDecals && m_pinFront && (g_pplayer->m_ptable->m_layback > 0))
      g_pplayer->m_pin3d.ExpandExtentsPlus(&m_rcScreen, rgv3DArrowTransformed, NULL, NULL, 4, fFalse);
   if (useDecals && m_pinBack && (g_pplayer->m_ptable->m_layback > 0))
      g_pplayer->m_pin3d.ExpandExtentsPlus(&m_rcScreen, rgv3DArrowTransformed2, NULL, NULL, 4, fFalse);

   if (useShadows)
   {
      g_pplayer->m_pin3d.ClearExtents(&m_rcScreenShadow, NULL, NULL);
      g_pplayer->m_pin3d.ExpandExtentsPlus(&m_rcScreenShadow, m_rgv3DShadow, NULL, NULL, 4, fFalse);

      if (fIntRectIntersect(m_rcScreen, m_rcScreenShadow))
      {
         m_rcScreen.left = min(m_rcScreen.left, m_rcScreenShadow.left);
         m_rcScreen.top = min(m_rcScreen.top, m_rcScreenShadow.top);
         m_rcScreen.right = max(m_rcScreen.right, m_rcScreenShadow.right);
         m_rcScreen.bottom = max(m_rcScreen.bottom, m_rcScreenShadow.bottom);
      }
      else
      {
         g_pplayer->InvalidateRect(&m_rcScreenShadow);
      }
   }
   g_pplayer->InvalidateRect(&m_rcScreen);
   if( useShadows && !useAntiAliasing )
   {
      DrawShadow();
   }
}
static const Material shadowmtrl = {1.f,1.f,1.f,1.f, 1.f,1.f,1.f,1.f, 0.f,0.f,0.f,0.f, 0.f,0.f,0.f,0.f, 0.f};
void Ball::DrawShadow()
{
   g_pplayer->m_pin3d.m_pd3dDevice->SetMaterial( (LPD3DMATERIAL7)&shadowmtrl);

   // special check if shadow falls on an object temp. removed due to strange crashs

   //Ball ballT;
   /*   ballT->x = x;
   ballT->y = y;
   ballT->z = z;
   ballT->vx = 200.0f;
   ballT->vy = -200.0f;
   ballT->vz = -200.0f;
   ballT->radius = 0;

   ballT->m_hittime = 1.0f;

   ballT->CalcBoundingRect();

   g_pplayer->GetShadowOctree().HitTestBall(ballT);

   float offsetx;
   float offsety;
   float shadowz;

   if (ballT->m_hittime < 1.0f) // shadow falls on an object
   {
   offsetx = ballT->m_hittime * 200.0f - 12.5f;
   offsety = ballT->m_hittime * -200.0f + 12.5f;
   shadowz = z + 0.1f - ballT->m_hittime * 200.0f;				
   }
   else // shadow is on the floor
   */
   float offsetx;
   float offsety;
   float shadowz;
   {
      offsetx = z*0.5f;
      offsety = z*-0.5f;
      shadowz = 0.1f; //pball->z - pball->radius + 0.1f;
   }

   const float shadowradius = radius*1.2f;
   const float shadowradiusX = shadowradius * g_pplayer->GetBallStretchX();
   const float shadowradiusY = shadowradius * g_pplayer->GetBallStretchY();
   const float inv_shadowradius = 0.5f/shadowradius;

   m_rgv3DShadow[0].x = x - shadowradiusX + offsetx;
   m_rgv3DShadow[0].y = y - shadowradiusY + offsety;
   m_rgv3DShadow[0].z = shadowz;

   m_rgv3DShadow[1].x = x + shadowradiusX + offsetx;
   m_rgv3DShadow[1].y = y - shadowradiusY + offsety;
   m_rgv3DShadow[1].z = shadowz;

   m_rgv3DShadow[2].x = x + shadowradiusX + offsetx;
   m_rgv3DShadow[2].y = y + shadowradiusY + offsety;
   m_rgv3DShadow[2].z = shadowz;

   m_rgv3DShadow[3].x = x - shadowradiusX + offsetx;
   m_rgv3DShadow[3].y = y + shadowradiusY + offsety;
   m_rgv3DShadow[3].z = shadowz;

   if (!fFrozen && m_rgv3DShadow[0].x <= g_pplayer->m_ptable->m_right && m_rgv3DShadow[2].y >= g_pplayer->m_ptable->m_top)
   {
      if (m_rgv3DShadow[2].x > g_pplayer->m_ptable->m_right)
      {
         const float newtu = (m_rgv3DShadow[2].x - g_pplayer->m_ptable->m_right) * inv_shadowradius;
         m_rgv3DShadow[2].tu = 1.0f-newtu;
         m_rgv3DShadow[1].tu = 1.0f-newtu;
         m_rgv3DShadow[2].x = g_pplayer->m_ptable->m_right;
         m_rgv3DShadow[1].x = g_pplayer->m_ptable->m_right;
      }

      if (m_rgv3DShadow[1].y < g_pplayer->m_ptable->m_top)
      {
         const float newtv = (g_pplayer->m_ptable->m_top - m_rgv3DShadow[1].y) * inv_shadowradius;
         m_rgv3DShadow[1].tv = newtv;
         m_rgv3DShadow[0].tv = newtv;
         m_rgv3DShadow[1].tv = g_pplayer->m_ptable->m_top;
         m_rgv3DShadow[0].tv = g_pplayer->m_ptable->m_top;
      }

      g_pplayer->m_pin3d.m_pd3dDevice->SetTexture(0, g_pplayer->m_pin3d.m_pddsShadowTexture);

      //g_pplayer->m_pin3d.m_pd3dDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
      g_pplayer->m_pin3d.m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, MY_D3DFVF_NOTEX2_VERTEX, m_rgv3DShadow, 4, (LPWORD)rgi0123, 4, NULL);
   }
}