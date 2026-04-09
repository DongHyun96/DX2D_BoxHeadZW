package Thing
{
   import World.*;
   import flash.events.*;
   import flash.geom.*;
   
   public class CThingObject extends CThingStatic
   {
      
      public function CThingObject(param1:Object, param2:* = null)
      {
         super();
         Inherit(param2);
         mSeed = nID;
         mAngle = new CThingAngle();
         mAngle.mAngle256 = RandomInt(256);
         SetPositionByInitObject(param1);
         mCollideRadius = param1.mCollideRadius ? Number(param1.mCollideRadius) : 0.5;
         mCollideHeight = mCollideRadius * 2;
         mAffectFlags = eAffectType.EXPLOSION | eAffectType.BULLET | eAffectType.ZOMBIEATTACK | eAffectType.VAPOURIZE | eAffectType.FIRE;
         MapAdd();
         Process = Process_Normal;
      }
      
      public function ApplyAffects() : Boolean
      {
         var _loc1_:CAffect = null;
         if(mAffects)
         {
            for each(_loc1_ in mAffects)
            {
               this["Affect_" + _loc1_.mName](_loc1_);
            }
            mAffects = null;
            if(mDead)
            {
               mAffectFlags = 0;
               Process = Process_Dispose;
               Destroyed();
               ClearCollide();
               MapRemove();
               return true;
            }
         }
         return false;
      }
      
      public function Process_Normal() : void
      {
         if(ApplyAffects())
         {
            return;
         }
         if(mStateID)
         {
            this[mStateID]();
         }
         ProcessChildren();
      }
      
      override public function get _CLASSID_() : String
      {
         return "CThingObject";
      }
      
      public function Destroyed() : void
      {
         AddThing(new CDestroySmoke(this,new CThingV3(0,0,mCollideHeight / 2)));
      }
   }
}

