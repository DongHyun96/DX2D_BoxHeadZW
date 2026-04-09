package Thing
{
   import Draw.*;
   import Landscape.*;
   import STC9.Sprite.*;
   import World.*;
   import flash.events.*;
   import flash.geom.*;
   
   public class CTurret_Mortor extends CThingTurret
   {
      
      public function CTurret_Mortor(param1:Object, param2:* = null, param3:* = null)
      {
         super(param1,param2,param3);
         if(isNaN(mAmmo) || mAmmo == 0)
         {
            mFireRate = CWTurret_Mortor._Upgrades[0].mTurretFireRate;
            mAutoFire = false;
            mTotalAmmo = mAmmo = CWTurret_Mortor._Upgrades[0].mTurretAmmo;
            mRange = CWTurret_Mortor._Upgrades[0].mRange;
         }
         mCollideRadius = 0.4;
         mCollideHeight = mCollideRadius * 2;
         SetCollide(mCellCurrent,eWMCCollideType.TURRET);
         mMaxLife = mLife = 120;
         if(!SpriteSheetExists(_CLASSID_))
         {
            AddSpriteSheet(_CLASSID_,new SheetTurret_Mortor(0,0),true);
         }
         AddSound("Shot",Turret_Mortor_Shot);
         SetState("State_GROWRADIUS","State_SEARCH");
         SetupRange(6,mRange);
         mTurnSpeed = CThingAngle.AsAngle256(1);
      }
      
      override public function Fire() : void
      {
         var _loc1_:CThingV3 = null;
         super.Fire();
         _loc1_ = SpriteSheet_GetModelOffset("SHOT1");
         AddThing(new CMortor(this,CAffect.BULLET(this,this,50),_loc1_,33.9 - 90,mTargetRange));
         AddThing(new CMortorMuzzleFlash(this,_loc1_,33.9 - 90));
         PlaySound("Shot");
      }
      
      override public function get mWeaponName() : String
      {
         return "Turret Mortor";
      }
      
      override public function get _CLASSID_() : String
      {
         return "CTurret_Mortor";
      }
   }
}

