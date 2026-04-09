package STC9.Sprite
{
   import STC9.Data.*;
   import flash.display.BitmapData;
   import flash.filters.*;
   import flash.geom.*;
   import flash.utils.ByteArray;
   
   public class CSpriteSheet
   {
      
      public var mAllFrames:Array;
      
      public var mAnimations:Object;
      
      public function CSpriteSheet()
      {
         super();
         mAnimations = new Object();
      }
      
      public static function AsBitmapData(param1:BitmapData) : CSpriteSheet
      {
         var _loc2_:CSpriteSheet = null;
         _loc2_ = new CSpriteSheet();
         _loc2_.CreateFrames(param1);
         return _loc2_;
      }
      
      public static function AsLibraryItem(param1:String) : CSpriteSheet
      {
         var _loc2_:CSpriteSheet = null;
         return new CSpriteSheet();
      }
      
      public function CreateFrames(param1:BitmapData) : void
      {
         var _loc2_:int = 0;
         var _loc3_:int = 0;
         var _loc4_:CByteArray = null;
         var _loc5_:int = 0;
         var _loc6_:ByteArray = null;
         var _loc7_:CByteArray = null;
         var _loc8_:int = 0;
         var _loc9_:Object = null;
         var _loc10_:CSpriteFrame = null;
         var _loc11_:Object = null;
         var _loc12_:Number = NaN;
         var _loc13_:CSpriteFrame = null;
         var _loc14_:Array = null;
         var _loc15_:Array = null;
         _loc2_ = 0;
         _loc3_ = 0;
         mAllFrames = new Array();
         _loc4_ = new CByteArray();
         _loc4_.writeBytes(param1.getPixels(new Rectangle(0,0,1,1)));
         _loc4_.position = 2;
         _loc5_ = _loc4_.readShort();
         _loc6_ = param1.getPixels(new Rectangle(_loc3_,_loc2_,param1.width,_loc5_));
         _loc7_ = new CByteArray();
         _loc7_.writeBytes(_loc6_,_loc4_.length);
         _loc7_.position = 0;
         _loc7_ = _loc7_.UnprotectAlpha();
         _loc7_.position = 0;
         _loc8_ = _loc7_.readShort();
         while(_loc8_)
         {
            _loc11_ = CSpriteFrame.UnpackBytes(_loc7_);
            if(!_loc11_)
            {
               break;
            }
            _loc12_ = 1;
            _loc11_.mArea.inflate(_loc12_,_loc12_);
            _loc13_ = new CSpriteFrame(_loc11_.mArea.width,_loc11_.mArea.height);
            _loc13_.mDisplacement = _loc11_.mArea.topLeft;
            _loc13_.mFrameIndex = _loc11_.mFrameIndex;
            _loc13_.mDirectionIndex = _loc11_.mDirectionIndex;
            _loc13_.mAnimID = _loc11_.mAnimID;
            _loc13_.mModelInfo = _loc11_.mModelInfo;
            _loc13_.copyPixels(param1,new Rectangle(_loc11_.mTopLeft.x,_loc11_.mTopLeft.y + _loc5_,_loc13_.width,_loc13_.height),new Point(_loc12_,_loc12_));
            _loc13_.applyFilter(_loc13_,_loc13_.rect,new Point(0,0),new GlowFilter(0,1,2,2,1,3));
            mAllFrames.push(_loc13_);
            _loc8_--;
         }
         _loc9_ = mAnimations;
         for each(_loc10_ in mAllFrames)
         {
            _loc14_ = _loc9_[_loc10_.mAnimID] = _loc9_[_loc10_.mAnimID] ? _loc9_[_loc10_.mAnimID] : new Array();
            _loc15_ = _loc14_[_loc10_.mDirectionIndex] = _loc14_[_loc10_.mDirectionIndex] ? _loc14_[_loc10_.mDirectionIndex] : new Array();
            _loc15_[_loc10_.mFrameIndex] = _loc10_;
         }
      }
      
      public function get _CLASSID_() : String
      {
         return "CSpriteSheet";
      }
   }
}

