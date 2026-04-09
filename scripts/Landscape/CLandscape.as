package Landscape
{
   import Draw.*;
   import Game.*;
   import Thing.*;
   import World.*;
   import adobe.utils.*;
   import flash.display.*;
   import flash.events.*;
   import flash.geom.*;
   import flash.ui.*;
   import flash.utils.*;
   
   public class CLandscape extends Sprite
   {
      
      public static const mScreenWidth:int = CGame.mScreenWidth;
      
      public static const mScreenHeight:int = CGame.mScreenHeight;
      
      public var mLSThingDraw:CLSThingDraw;
      
      public var mRenderFullArea:Rectangle;
      
      public var mLSDraw:CLSDraw;
      
      public var mPosition:Point;
      
      public function CLandscape()
      {
         super();
         mPosition = new Point(0,0);
         scrollRect = new Rectangle(0,0,mScreenWidth,mScreenHeight);
         mLSDraw = new CLSDraw(new CGame.mLevelData.initClass(),mScreenWidth,mScreenHeight);
         mLSThingDraw = new CLSThingDraw();
         addChild(mLSDraw);
         mRenderFullArea = new Rectangle(0,0,mLSDraw.mMapWidth * CLSDraw.xProjection,mLSDraw.mMapHeight * CLSDraw.yProjection);
         mRenderFullArea.left += CLSDraw.xProjection * 2;
         mRenderFullArea.top += CLSDraw.yProjection * 2;
      }
      
      public function get mPlayerInitPositions() : Array
      {
         return mLSDraw.mPlayerInitPositions;
      }
      
      public function get mThingInitPositions() : Array
      {
         return mLSDraw.mThingInitPositions;
      }
      
      public function get mMapSize() : Point
      {
         return mLSDraw.mMapSize;
      }
      
      public function GetAltitudeAt(param1:Number, param2:Number) : Number
      {
         return 0;
      }
      
      public function Draw() : void
      {
         var _loc1_:Point = null;
         var _loc2_:Rectangle = null;
         _loc1_ = new Point();
         _loc1_.x = int(Math.max(mRenderFullArea.x,Math.min(mRenderFullArea.width - mScreenWidth,mPosition.x * CLSDraw.xProjection)));
         _loc1_.y = int(Math.max(mRenderFullArea.y,Math.min(mRenderFullArea.height - mScreenHeight,mPosition.y * CLSDraw.yProjection)));
         _loc2_ = new Rectangle(_loc1_.x,_loc1_.y,mScreenWidth,mScreenHeight);
         mLSDraw.Draw(_loc2_,mLSThingDraw);
      }
      
      override public function toString() : String
      {
         return _CLASSID_;
      }
      
      public function TrackThing(param1:CThingDraw, param2:Point) : void
      {
         var _loc3_:CThingV3 = null;
         var _loc4_:CThingV3 = null;
         if(!param1)
         {
            return;
         }
         _loc3_ = param1.mPosition;
         _loc4_ = new CThingV3(mPosition.x + mScreenWidth / CLSDraw.xProjection / 2,mPosition.y + mScreenHeight / CLSDraw.yProjection / 2);
         _loc4_.Sub(_loc3_);
         Scroll(-_loc4_.x + param2.x,-_loc4_.y + param2.y);
      }
      
      public function QGetAltitudeAt(param1:Number, param2:Number) : Number
      {
         return 0;
      }
      
      public function get _CLASSID_() : String
      {
         return "CLandscape";
      }
      
      public function Scroll(param1:Number, param2:Number) : void
      {
         mPosition.x += param1;
         mPosition.y += param2;
      }
      
      public function Dispose() : void
      {
         mLSDraw.Dispose();
         removeChild(mLSDraw);
         mLSDraw = null;
      }
   }
}

