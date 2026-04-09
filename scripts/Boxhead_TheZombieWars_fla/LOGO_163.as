package Boxhead_TheZombieWars_fla
{
   import Game.CGlobalData;
   import flash.display.MovieClip;
   import flash.display.SimpleButton;
   import flash.events.Event;
   import flash.events.MouseEvent;
   import flash.events.TimerEvent;
   import flash.utils.Timer;
   
   [Embed(source="/_assets/assets.swf", symbol="symbol797")]
   public dynamic class LOGO_163 extends MovieClip
   {
      
      public var bTButton:SimpleButton;
      
      public var nTimer:Timer;
      
      public var mFinished:Boolean;
      
      public function LOGO_163()
      {
         super();
         addFrameScript(0,frame1);
      }
      
      public function e_ANIMATE(param1:TimerEvent) : void
      {
         this.nextFrame();
         if(this.currentFrame == this.totalFrames)
         {
            nTimer.removeEventListener("timer",e_ANIMATE);
            dispatchEvent(new Event("FINISHED"));
         }
      }
      
      public function e_GOTOURL(param1:MouseEvent) : void
      {
         CGlobalData.OpenURL_GamesWebsite();
      }
      
      internal function frame1() : *
      {
         stop();
         mFinished = false;
         bTButton.addEventListener(MouseEvent.MOUSE_UP,e_GOTOURL);
         nTimer = new Timer(1000 / 30);
         nTimer.addEventListener("timer",e_ANIMATE);
      }
      
      public function START() : *
      {
         visible = true;
         nTimer.start();
      }
   }
}

