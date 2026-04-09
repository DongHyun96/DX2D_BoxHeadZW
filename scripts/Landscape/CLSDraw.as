package Landscape
{
   import Draw.*;
   import Game.*;
   import STC9.Bitmap.*;
   import STC9.Filters.*;
   import STC9.Math.*;
   import STC9.System.Profiler.*;
   import Thing.*;
   import World.*;
   import flash.display.*;
   import flash.events.*;
   import flash.filters.*;
   import flash.geom.*;
   import flash.text.*;
   import flash.utils.*;
   
   public class CLSDraw extends Sprite
   {
      
      private static var renderTopLeft:Point;
      
      public static const xDrawSize:int = 40;
      
      public static const yDrawSize:int = 28;
      
      public static const xProjection:int = xDrawSize;
      
      public static const yProjection:int = yDrawSize;
      
      public static const zProjection:Number = Math.SQRT1_2 * xDrawSize;
      
      public static const sProjection:Number = yProjection / xProjection;
      
      private var bmdBlood:Array;
      
      private var bPlane:Bitmap;
      
      private var bmdFloor:CBitmapData;
      
      public var mPlayerInitPositions:Array;
      
      private var mLevelOrigin:Point;
      
      private var zPoint:Point = new Point(0,0);
      
      public var mThingInitPositions:Array;
      
      private var bmdDecalCover:CBitmapData;
      
      private var yPitch:int;
      
      private var bmdPlane:CBitmapData;
      
      private var mSFXLayer:CLSEffects;
      
      private var bmdCovers:Array;
      
      private var drawLayer:Sprite;
      
      private var mcLevel:MovieClip;
      
      private var xPitch:int;
      
      private var mTick:int = 0;
      
      private var bmdArrayActive:Boolean;
      
      private var bmdDecal:CBitmapData;
      
      private var mAllCells:Array;
      
      private var bmdCover:CBitmapData;
      
      private var screenWidth:int;
      
      private var screenHeight:int;
      
      private var bmdFloors:Array;
      
      private var bmdScorch:Array;
      
      public var mCells:Array;
      
      public function CLSDraw(param1:*, param2:int, param3:int)
      {
         super();
         screenWidth = param2;
         screenHeight = param3;
         CompileLevel(param1);
         CreateDecals();
         addChild(bPlane = new Bitmap(bmdPlane = new CBitmapData(screenWidth,screenHeight,false,0)));
         mSFXLayer = new CLSEffects();
         mSFXLayer.mActive = CGlobalData.GetData("Quality") != "low";
      }
      
      public static function ProjectionPoint(param1:Point) : Point
      {
         return new Point(param1.x * xProjection,param1.y * yProjection);
      }
      
      public static function ProjectionXYZ(param1:Number, param2:Number, param3:Number = 0) : Point
      {
         return new Point(param1 * xProjection,param2 * yProjection - param3 * zProjection);
      }
      
      public static function ProjectionThing_Screen(param1:CThingV3) : Point
      {
         return new Point(param1.x * xProjection - renderTopLeft.x,param1.y * yProjection - param1.z * zProjection - renderTopLeft.y);
      }
      
      public static function QProjectionXYZ(param1:Number, param2:Number, param3:Number = 0) : Point
      {
         return new Point(Math.floor(param1 * xProjection),Math.floor(param2 * yProjection - param3 * zProjection));
      }
      
      public static function ProjectionThing(param1:CThingV3) : Point
      {
         return new Point(param1.x * xProjection,param1.y * yProjection - param1.z * zProjection);
      }
      
      public function DrawBloodMulti(param1:Number, param2:Number) : void
      {
         var _loc3_:int = 0;
         _loc3_ = 3;
         while(_loc3_)
         {
            DrawBlood(param1 + (Math.random() - 0.5) * 0.75,param2 + (Math.random() - 0.5) * 0.75);
            _loc3_--;
         }
      }
      
      private function InvertPlane(param1:CBitmapData) : void
      {
         var _loc2_:Array = null;
         var _loc3_:Number = NaN;
         _loc2_ = new Array();
         _loc3_ = 0;
         while(_loc3_ <= 255)
         {
            _loc2_[_loc3_] = 255 - _loc3_ << 24;
            _loc3_++;
         }
         param1.paletteMap(param1,param1.rect,zPoint,null,null,null,_loc2_);
      }
      
      public function ShowGrid(param1:CBitmapData) : void
      {
         var _loc2_:int = 0;
         var _loc3_:int = 0;
         _loc2_ = 0;
         while(_loc2_ < param1.height)
         {
            _loc3_ = 0;
            while(_loc3_ < param1.width)
            {
               param1.fillRect(new Rectangle(_loc3_,_loc2_,1,yDrawSize),0);
               param1.fillRect(new Rectangle(_loc3_,_loc2_,xDrawSize,1),0);
               _loc3_ += xDrawSize;
            }
            _loc2_ += yDrawSize;
         }
      }
      
      private function InitMap() : Array
      {
         var _loc1_:int = 0;
         var _loc2_:int = 0;
         var _loc3_:Array = null;
         var _loc4_:Object = null;
         mCells = new Array();
         mAllCells = new Array();
         _loc1_ = 0;
         while(_loc1_ < yPitch)
         {
            _loc3_ = mCells[_loc1_] = new Array();
            _loc2_ = 0;
            while(_loc2_ < xPitch)
            {
               _loc3_.push({"mCollide":0});
               _loc2_++;
            }
            mAllCells = mAllCells.concat(_loc3_);
            _loc1_++;
         }
         _loc1_ = 0;
         while(_loc1_ < yPitch)
         {
            mCells[_loc1_][0].mCollide = mCells[_loc1_][xPitch - 1].mCollide = mCells[_loc1_][xPitch - 1].mCollide | (eWMCCollideType.EDGE | eWMCCollideType.SOLID);
            mCells[_loc1_][1].mCollide = mCells[_loc1_][xPitch - 2].mCollide = mCells[_loc1_][xPitch - 2].mCollide | eWMCCollideType.EDGE;
            _loc1_++;
         }
         _loc2_ = 0;
         while(_loc2_ < xPitch)
         {
            mCells[0][_loc2_].mCollide = mCells[yPitch - 1][_loc2_].mCollide = mCells[yPitch - 1][_loc2_].mCollide | (eWMCCollideType.EDGE | eWMCCollideType.SOLID);
            mCells[1][_loc2_].mCollide = mCells[yPitch - 2][_loc2_].mCollide = mCells[yPitch - 2][_loc2_].mCollide | eWMCCollideType.EDGE;
            _loc2_++;
         }
         return mCells;
      }
      
      public function CompileLevel(param1:*) : *
      {
         var _loc2_:Array = null;
         var _loc3_:Array = null;
         var _loc4_:Array = null;
         var _loc5_:Array = null;
         var _loc6_:Array = null;
         var _loc7_:int = 0;
         var _loc8_:CLSCtrlFloor = null;
         var _loc9_:CLSCtrlCover = null;
         var _loc10_:Matrix = null;
         var _loc11_:* = undefined;
         mcLevel = param1;
         FillMapDimensions(mcLevel._Size);
         InitMap();
         _loc2_ = new Array();
         _loc3_ = new Array();
         _loc4_ = new Array();
         _loc5_ = new Array();
         _loc6_ = new Array();
         mPlayerInitPositions = new Array();
         mThingInitPositions = new Array();
         _loc7_ = mcLevel.numChildren - 1;
         while(_loc7_ >= 0)
         {
            _loc11_ = mcLevel.getChildAt(_loc7_);
            switch(_loc11_.toString())
            {
               default:
                  _loc5_.push(_loc11_);
                  break;
               case "CLSCtrlSize":
                  _loc5_.push(_loc11_);
                  _loc11_.visible = false;
                  break;
               case "CLSCtrlCover":
                  _loc2_.push(_loc11_);
                  CompileObject(_loc11_);
                  break;
               case "CLSCtrlFloor":
                  _loc3_.push(_loc11_);
                  break;
               case "CLSCtrlEffect":
                  _loc4_.push(_loc11_);
                  break;
               case "CLSCtrlPlayer":
                  AddInitPlayer(_loc11_);
                  break;
               case "CLSCtrlBarricade":
                  AddInitThing(_loc11_,CBarricade);
                  break;
               case "CLSCtrlTree":
                  AddInitThing(_loc11_,CTree);
                  break;
               case "CLSCtrlTurretMortor":
                  AddInitThing(_loc11_,CTurret_Mortor);
                  break;
               case "CLSCtrlTurretMG":
                  AddInitThing(_loc11_,CTurret_MG);
                  break;
               case "CLSCtrlTurretRockets":
                  AddInitThing(_loc11_,CTurret_Rockets);
                  break;
               case "CLSCtrlRock":
                  AddInitThing(_loc11_,CRock);
                  break;
               case "CLSCtrlCollision":
                  SetCollisionArea(new Point(_loc11_.x,_loc11_.y),new Point(_loc11_.x + _loc11_.width,_loc11_.y + _loc11_.height));
                  _loc5_.push(_loc11_);
                  _loc11_.visible = false;
            }
            _loc7_--;
         }
         mcLevel._Size.visible = false;
         _loc10_ = new Matrix(1,0,0,1,-mcLevel._Size.x,-mcLevel._Size.y);
         SetVisibleThings(mPlayerInitPositions,false);
         SetVisibleThings(mThingInitPositions,false);
         bmdFloor = new CBitmapData(mcLevel._Size.width,mcLevel._Size.height,false,16711935);
         bmdFloor.draw(mcLevel,_loc10_);
         SetVisible(_loc5_,false);
         SetVisible(_loc3_,false);
         SetVisible(_loc4_,false);
         bmdDecalCover = new CBitmapData(mcLevel._Size.width,mcLevel._Size.height,true,0);
         bmdDecalCover.draw(mcLevel,_loc10_);
         InvertPlane(bmdDecalCover);
         for each(_loc9_ in _loc2_)
         {
            ActivateCoverMask(_loc9_);
         }
         bmdCover = new CBitmapData(mcLevel._Size.width,mcLevel._Size.height,true,0);
         bmdCover.draw(mcLevel,_loc10_);
         mcLevel = null;
      }
      
      private function CompileObject(param1:*) : void
      {
         var _loc2_:int = 0;
         var _loc3_:* = undefined;
         var _loc4_:Point = null;
         var _loc5_:Point = null;
         param1.mcMask = new MovieClip();
         _loc2_ = param1.numChildren - 1;
         while(_loc2_ >= 0)
         {
            _loc3_ = param1.getChildAt(_loc2_);
            switch(_loc3_.toString())
            {
               case "CLSCtrlCollision":
                  _loc3_.visible = false;
                  _loc4_ = param1.localToGlobal(new Point(_loc3_.x,_loc3_.y));
                  _loc4_ = mcLevel.globalToLocal(_loc4_);
                  _loc5_ = param1.localToGlobal(new Point(_loc3_.x + _loc3_.width,_loc3_.y + _loc3_.height));
                  _loc5_ = mcLevel.globalToLocal(_loc5_);
                  SetCollisionArea(_loc4_,_loc5_);
                  break;
               case "CLSCtrlCoverMask":
                  param1.removeChild(_loc3_);
                  param1.mcMask.addChild(_loc3_);
            }
            _loc2_--;
         }
      }
      
      private function SetVisibleThings(param1:Array, param2:Boolean = false) : void
      {
         var _loc3_:Object = null;
         for each(_loc3_ in param1)
         {
            _loc3_.dObject.visible = param2;
         }
      }
      
      private function AddInitThing(param1:*, param2:Class) : void
      {
         var _loc3_:Point = null;
         _loc3_ = new Point(Math.round((param1.x - mcLevel._Size.x) / xDrawSize) + 0.5,Math.round((param1.y - mcLevel._Size.y) / yDrawSize) + 0.5);
         mThingInitPositions.push({
            "mPosition":new CThingV3(_loc3_.x,_loc3_.y),
            "initClass":param2,
            "dObject":param1
         });
      }
      
      public function get mMapSize() : Point
      {
         return new Point(xPitch,yPitch);
      }
      
      public function Draw(param1:Rectangle, param2:CLSThingDraw) : void
      {
         var _loc3_:Rectangle = null;
         renderTopLeft = param1.topLeft;
         CThingDraw.mSFXLayer = mSFXLayer;
         CThingDraw.bmdPlane = bmdPlane;
         CThingAudio.mAudioSize = new Point(param1.width / CLSDraw.xProjection,param1.height / CLSDraw.yProjection);
         CThingAudio.mAudioCentre = new CThingV3((param1.x + param1.width / 2) / CLSDraw.xProjection,(param1.y + param1.height / 2) / CLSDraw.yProjection);
         _loc3_ = param1.clone();
         CGame.mFPS.StartProfile("FLOOR_Draw");
         bmdPlane.copyPixels(bmdFloor,_loc3_,zPoint);
         CGame.mFPS.StopProfile("FLOOR_Draw");
         CGame.mFPS.StartProfile("THING_Draw");
         param2.Draw(param1);
         CGame.mFPS.StopProfile("THING_Draw");
         CGame.mFPS.StartProfile("SFX_Draw");
         mSFXLayer.Render(param1,bmdPlane);
         CGame.mFPS.StopProfile("SFX_Draw");
         CGame.mFPS.StartProfile("COVER_Draw");
         bmdPlane.copyPixels(bmdCover,_loc3_,zPoint);
         CGame.mFPS.StopProfile("COVER_Draw");
         CGame.mHUD.Draw(param1,bmdPlane);
      }
      
      private function ActivateCoverMask(param1:*) : void
      {
         if(param1.mcMask.numChildren)
         {
            param1.addChild(param1.mcMask);
            param1.mask = param1.mcMask;
         }
      }
      
      override public function toString() : String
      {
         return _CLASSID_;
      }
      
      public function get mMapWidth() : int
      {
         return xPitch;
      }
      
      private function AddInitPlayer(param1:*) : void
      {
         var _loc2_:Point = null;
         _loc2_ = new Point(Math.round((param1.x - mcLevel._Size.x) / xDrawSize) + 0.5,Math.round((param1.y - mcLevel._Size.y) / yDrawSize) + 0.5);
         mPlayerInitPositions.push({
            "mPosition":new CThingV3(_loc2_.x,_loc2_.y),
            "dObject":param1
         });
      }
      
      private function CreateDecals() : void
      {
         var _loc1_:CBitmapData = null;
         var _loc2_:int = 0;
         var _loc3_:BloodDecal = null;
         var _loc4_:ScorchMarkDecal = null;
         _loc3_ = new BloodDecal();
         bmdBlood = new Array();
         _loc2_ = 0;
         while(_loc2_ < _loc3_._Content.totalFrames)
         {
            _loc3_._Content.gotoAndStop(_loc2_ + 1);
            bmdBlood.push(_loc1_ = new CBitmapData(Math.ceil(_loc3_.width),Math.ceil(_loc3_.height),true,0));
            _loc1_.draw(_loc3_);
            _loc2_++;
         }
         _loc4_ = new ScorchMarkDecal();
         bmdScorch = new Array();
         _loc2_ = 0;
         while(_loc2_ < _loc4_._Content.totalFrames)
         {
            _loc4_._Content.gotoAndStop(_loc2_ + 1);
            bmdScorch.push(_loc1_ = new CBitmapData(Math.ceil(_loc4_.width),Math.ceil(_loc4_.height),true,0));
            _loc1_.draw(_loc4_);
            _loc2_++;
         }
      }
      
      private function FillMapDimensions(param1:*) : void
      {
         xPitch = Math.round(param1.width / xDrawSize);
         yPitch = Math.round(param1.height / yDrawSize);
         mLevelOrigin = new Point(param1.x,param1.y);
      }
      
      public function DrawScorch(param1:Number, param2:Number) : void
      {
         DrawDecal(bmdScorch[int(Math.random() * bmdScorch.length)],param1,param2);
      }
      
      public function get _CLASSID_() : String
      {
         return "CLSDraw";
      }
      
      public function get mMapHeight() : int
      {
         return yPitch;
      }
      
      private function SetCollisionArea(param1:Point, param2:Point) : void
      {
         var _loc3_:int = 0;
         var _loc4_:int = 0;
         var _loc5_:Array = null;
         param1 = param1.subtract(mLevelOrigin);
         param2 = param2.subtract(mLevelOrigin);
         param1.x = Math.round(param1.x / xDrawSize);
         param1.y = Math.round(param1.y / yDrawSize);
         param2.x = Math.round(param2.x / xDrawSize);
         param2.y = Math.round(param2.y / yDrawSize);
         _loc3_ = param1.y;
         while(_loc3_ < param2.y)
         {
            if(!(_loc3_ < 0 || _loc3_ >= yPitch))
            {
               _loc5_ = mCells[_loc3_];
               _loc4_ = param1.x;
               while(_loc4_ < param2.x)
               {
                  if(!(_loc4_ < 0 || _loc4_ >= xPitch))
                  {
                     _loc5_[_loc4_].mCollide = eWMCCollideType.SOLID;
                  }
                  _loc4_++;
               }
            }
            _loc3_++;
         }
      }
      
      private function SetVisible(param1:Array, param2:Boolean = false) : void
      {
         var _loc3_:DisplayObject = null;
         for each(_loc3_ in param1)
         {
            _loc3_.visible = param2;
         }
      }
      
      public function DrawDecal(param1:CBitmapData, param2:Number, param3:Number) : void
      {
         var _loc4_:Point = null;
         _loc4_ = new Point(param2 * xProjection - param1.width / 2,param3 * yProjection - param1.height / 2);
         bmdFloor.copyPixels(param1,param1.rect,_loc4_,bmdDecalCover,_loc4_,false);
      }
      
      public function DrawBlood(param1:Number, param2:Number) : void
      {
         DrawDecal(bmdBlood[int(Math.random() * bmdBlood.length)],param1,param2);
      }
      
      public function Dispose() : void
      {
         removeChild(bPlane);
         bmdPlane.dispose();
         bmdFloor.dispose();
         bmdCover.dispose();
         bmdDecalCover.dispose();
         bmdPlane = bmdFloor = bmdCover = bmdDecalCover = null;
         mSFXLayer.Dispose();
         mSFXLayer = null;
      }
   }
}

