package Thing
{
   import Draw.*;
   import Landscape.*;
   import STC9.Sprite.*;
   import World.*;
   import flash.events.*;
   import flash.geom.*;
   
   public class CRock extends CThingObject
   {
      
      public function CRock(param1:Object, param2:* = null)
      {
         param1.mCollideRadius = 0.5;
         super(param1);
         mMaxLife = mLife = 1000;
         mAudioMaterial = "METAL";
         SetCollide(mCellCurrent,eWMCCollideType.ROCK);
         if(!MCAnimationExists(_CLASSID_))
         {
            AddMCAnimation(_CLASSID_,new Object_Rock());
         }
         SetState("State_GROWRADIUS");
         mAnimationFrame = Random();
      }
      
      override public function get _CLASSID_() : String
      {
         return "CRock";
      }
   }
}

