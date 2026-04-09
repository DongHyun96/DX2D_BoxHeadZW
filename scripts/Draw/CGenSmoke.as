package Draw
{
   import STC9.Bitmap.*;
   import flash.display.*;
   import flash.events.*;
   import flash.geom.*;
   import flash.utils.getDefinitionByName;
   
   public class CGenSmoke extends CSprite
   {
      
      private var renderContainer:Sprite;
      
      private var renderScalar:MovieClip;
      
      private var dScaleSpeed:Number = 0.1;
      
      private var particles:Array;
      
      private var m:Matrix = new Matrix();
      
      private var rDelta:Number = 8;
      
      private var genSteps:int;
      
      private var particleClasses:Array;
      
      private var pCounter:int = 0;
      
      private var dDeltaDecay:Number = 0.99;
      
      public function CGenSmoke(param1:Array, param2:Number = 1, param3:int = 5)
      {
         super();
         genSteps = param3;
         particleClasses = param1;
         renderScalar = new MovieClip();
         renderContainer = new Sprite();
         renderScalar.addChild(renderContainer);
         renderContainer.scaleX = renderContainer.scaleY = 0.1 * param2;
         mFrames = Render();
      }
      
      public function Draw(param1:CBitmapData, param2:int, param3:int, param4:Number = 0, param5:Number = 1, param6:Number = 1) : void
      {
         var _loc7_:int = 0;
         var _loc8_:CSpriteBMD = null;
         _loc7_ = param4 * mFrames.length % mFrames.length;
         _loc8_ = mFrames[_loc7_];
         _loc8_.Draw(param1,param2,param3,param5,param6);
      }
      
      private function ProcessParticles(param1:int) : Boolean
      {
         var _loc2_:MovieClip = null;
         while(param1)
         {
            if(particles.length < 10 && pCounter > 0)
            {
               if(Math.random() > 0.5)
               {
                  AddParticle();
               }
            }
            for each(_loc2_ in particles.slice())
            {
               _loc2_.scaleX += (_loc2_.dScale - _loc2_.scaleX) * dScaleSpeed;
               _loc2_.scaleY = _loc2_.scaleX;
               _loc2_.x += _loc2_.dDelta.x;
               _loc2_.y += _loc2_.dDelta.y;
               _loc2_.dDelta.normalize(_loc2_.dDelta.length * dDeltaDecay);
               _loc2_.dDelta.y -= 0.25;
               _loc2_.rotation += _loc2_.dRotation;
               _loc2_.dRotation *= 1.1;
               _loc2_.nextFrame();
               if(_loc2_.currentFrame == _loc2_.totalFrames)
               {
                  particles.splice(particles.indexOf(_loc2_),1);
                  renderContainer.removeChild(_loc2_);
                  if(particles.length == 0)
                  {
                     return false;
                  }
               }
            }
            param1--;
         }
         return true;
      }
      
      private function AddParticle() : void
      {
         var _loc1_:* = undefined;
         _loc1_ = new particleClasses[int(Math.random() * particleClasses.length)]();
         particles.push(_loc1_);
         renderContainer.addChildAt(_loc1_,0);
         _loc1_.scaleX = _loc1_.scaleY = 0.5 + Math.random() * 0.5;
         _loc1_.dDelta = m.transformPoint(new Point(rDelta * (1 + Math.random() * 0.5),0));
         _loc1_.dDelta.y *= 28 / 40;
         _loc1_.dScale = Math.random() * 0.4 + 1.6;
         _loc1_.dRotation = Math.random() * 1 - 0.5;
         _loc1_.stop();
         m.rotate(Math.random() * (Math.PI * 2));
         --pCounter;
      }
      
      override public function get _CLASSID_() : String
      {
         return "CGenSmoke";
      }
      
      public function DrawGFX(param1:Graphics, param2:int, param3:int, param4:Number = 0, param5:Number = 1) : void
      {
         var _loc6_:int = 0;
         var _loc7_:CSpriteBMD = null;
         _loc6_ = param4 * mFrames.length % mFrames.length;
         _loc7_ = mFrames[_loc6_];
         _loc7_.DrawGFX(param1,param2,param3,param5);
      }
      
      private function Render() : Array
      {
         var _loc1_:Array = null;
         var _loc2_:* = undefined;
         particles = new Array();
         pCounter = 10;
         _loc1_ = new Array();
         _loc2_ = new CSpriteBMD(100,100);
         while(ProcessParticles(genSteps))
         {
            _loc1_.push(CSpriteBMD.asDisplayObject(renderScalar));
         }
         return _loc1_;
      }
   }
}

