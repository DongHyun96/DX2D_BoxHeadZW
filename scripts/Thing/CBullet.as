package Thing
{
   import Landscape.*;
   import flash.display.*;
   import flash.events.*;
   import flash.geom.*;
   
   public class CBullet extends CThingShot
   {
      
      public var mColor:uint = 4294901760;
      
      public function CBullet(param1:*, param2:CAffect, param3:CThingV3, param4:int = 1, param5:Number = 16, param6:Number = 0.2)
      {
         var _loc7_:Object = null;
         var _loc8_:Array = null;
         var _loc9_:* = undefined;
         mColor = 4294901760;
         super(param1,param2);
         Process = Process_Normal;
         mSpeed = param5;
         mDelta = mAngle.mDelta.ScaleN(mSpeed);
         mCollideRadius = param6;
         mLife = 1;
         mPosition.Add(param3);
         MapAdd();
         _loc7_ = GetThingsAngle(mSpeed,mCollideRadius,eAffectType.BULLET,param4);
         _loc8_ = _loc7_.mThings;
         if(_loc8_.length)
         {
            for each(_loc9_ in _loc8_)
            {
               _loc9_.AddAffect(mAffect);
            }
            mShotDestination = _loc8_[0].mPosition.Clone().Add(param3);
         }
         else if(_loc7_.mImpactPOI)
         {
            mShotDestination = new CThingV3(_loc7_.mImpactPOI.x,_loc7_.mImpactPOI.y,mPosition.z);
         }
         else
         {
            mShotDestination = mPosition.Clone().Add(mDelta);
         }
         AddDrawDefault();
      }
      
      public function Process_Normal() : *
      {
         if(--mLife < 0)
         {
            Dispose();
            return;
         }
      }
      
      override public function get _CLASSID_() : String
      {
         return "CBullet";
      }
      
      override public function Render() : void
      {
         mSFXLayer.DrawBulletSmoke(mRenderPosition,RenderPosition(mShotDestination));
         mDrawn = false;
      }
   }
}

