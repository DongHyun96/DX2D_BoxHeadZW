package Thing
{
   import Draw.*;
   import Landscape.*;
   import STC9.Sprite.*;
   import World.*;
   import flash.events.*;
   import flash.geom.*;
   
   public class CDevil extends CThingPeep
   {
      
      private var mRShot:CThingShot;
      
      public function CDevil(param1:Object = null)
      {
         super(param1,SheetDevil);
         mName = "DEVIL";
         mSpeed = 10 / 100;
         mAngle.mDirection = 2;
         SetAnimation("stand");
         SetCollide(mCellCurrent,eWMCCollideType.PEEP);
         mMaxLife = mLife = 1000;
         mAffectThreshold = 1;
         mScore = 1000;
         SetState("State_NAVIGATE");
         mMass = 0.2;
         mCollideRadius = 0.4;
         mCollideHeight = 1400 / 500 * mCollideRadius;
         mAffectFlags &= ~eAffectType.PUSH;
         mAffectFlags &= ~eAffectType.ZOMBIEATTACK;
         AddSound("Attack",Devil_Attack);
         mName = "Devil";
      }
      
      public static function PrepareClass() : void
      {
         var _loc1_:CThingDraw = null;
         _loc1_ = new CThingDraw();
         if(!_loc1_.SpriteSheetExists("CDevil"))
         {
            _loc1_.AddSpriteSheet("CDevil",new SheetDevil(0,0));
         }
         _loc1_.Dispose();
      }
      
      public function State_ATTACK_Exit() : void
      {
         mAttackCounter = SECONDS * 1 / mSpeedScale;
      }
      
      override public function State_HIT_Enter() : void
      {
         super.State_HIT_Enter();
         mAttackCounter = SECONDS * 1;
      }
      
      override public function Process_Normal() : *
      {
         var _loc1_:Number = NaN;
         super.Process_Normal();
         if(mStateID != "State_ATTACK")
         {
            if(mExists)
            {
               _loc1_ = mCollideRadius;
               mCollideRadius *= 1.5;
               AffectRadius(CAffect.PUSH(this,0.5),100,mAngle.mDelta.ScaleN(_loc1_));
               mCollideRadius = _loc1_;
            }
         }
      }
      
      public function State_ATTACK_Enter() : void
      {
         mAngleTarget = CThingAngle.AsCThingV3(mTarget.mPosition.Clone().Sub(mPosition));
         if(mAngleTarget.mAngle256 != mAngle.mAngle256)
         {
            SetState("State_TURN",mStateID);
            return;
         }
         SetAnimation("attack");
         PlaySound("Attack");
      }
      
      override public function State_NAVIGATE() : void
      {
         if(!mCanProcess)
         {
            return;
         }
         if(mAttackCounter == 0)
         {
            if(mCellCurrent.mNavTarget)
            {
               if(mCellCurrent.mNavIndex <= 10)
               {
                  if(mCellCurrent.mNavTarget.mExists)
                  {
                     mTarget = mCellCurrent.mNavTarget;
                     if(mPosition.Distance2D(mTarget.mPosition) <= 6)
                     {
                        SetState("State_ATTACK");
                        return;
                     }
                  }
               }
            }
         }
         super.State_NAVIGATE();
      }
      
      override public function get _CLASSID_() : String
      {
         return "CDevil";
      }
      
      public function State_ATTACK() : void
      {
         if(AnimateEnd())
         {
            if(!mRShot)
            {
               AddThing(mRShot = new CFlameLine(this,new CThingV3(0,0.1,0)));
            }
            else if(mRShot.mDisposed)
            {
               SetState("State_WAIT",mBaseStateID);
               mStateCount = 1;
               mRShot = null;
            }
         }
      }
   }
}

