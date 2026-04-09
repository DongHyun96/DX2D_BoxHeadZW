package Thing
{
   import Draw.*;
   import Game.*;
   import Landscape.*;
   import STC9.Sprite.*;
   import World.*;
   import flash.events.*;
   import flash.geom.*;
   
   public class CBarrel extends CThingObject
   {
      
      private var rInitObject:Object;
      
      public function CBarrel(param1:Object, param2:* = null)
      {
         param1.mCollideRadius = 0.4;
         super(rInitObject = param1,param2);
         this.mCollideID = nID;
         mMaxLife = mLife = 1;
         mAudioMaterial = "METAL";
         SetCollide(mCellCurrent,eWMCCollideType.BARREL);
         if(!SpriteSheetExists(_CLASSID_))
         {
            AddSpriteSheet(_CLASSID_,new SheetBarrel(0,0));
         }
         mAffectFlags &= ~eAffectType.ZOMBIEATTACK;
         SetState("State_GROWRADIUS");
      }
      
      override public function Affect_ZOMBIEATTACK(param1:CAffect) : void
      {
         mLife -= param1.mDamage;
      }
      
      override public function AddKill() : *
      {
         CGlobalData.AddStat("Barrel kills",1,"ADD");
         return this;
      }
      
      override public function get _CLASSID_() : String
      {
         return "CBarrel";
      }
      
      override public function Destroyed() : void
      {
         var _loc1_:CExplosion = null;
         this.mCollideID = mOwner.mCollideID;
         AddThing(_loc1_ = new CExplosion(this,new CThingV3(0,0,mCollideHeight / 2)));
         if(rInitObject.mClusterShells)
         {
            _loc1_.AddClusterShells(rInitObject.mClusterShells);
         }
         if(rInitObject.mBiggerBang)
         {
            _loc1_.AddBiggerBang(rInitObject.mBiggerBang);
         }
      }
   }
}

