package Boxhead_TheZombieWars_fla
{
   import CPMStar.*;
   import Game.CGlobalData;
   import IO.CMochiBot;
   import IO.CProtection;
   import STC9.String.CString;
   import adobe.utils.*;
   import flash.accessibility.*;
   import flash.display.*;
   import flash.errors.*;
   import flash.events.*;
   import flash.external.*;
   import flash.filters.*;
   import flash.geom.*;
   import flash.media.*;
   import flash.net.*;
   import flash.printing.*;
   import flash.system.*;
   import flash.text.*;
   import flash.ui.*;
   import flash.utils.*;
   import flash.xml.*;
   
   public dynamic class MainTimeline extends MovieClip
   {
      
      public var tfDebug:TextField;
      
      public var tfVersion:TextField;
      
      public var currentPercentage:int;
      
      public var mcLoadingText:MovieClip;
      
      public var mcLoaderBar:MovieClip;
      
      public var bMoreGames:SimpleButton;
      
      public var mcCountDown:MovieClip;
      
      public var mcLogo:SimpleButton;
      
      public var startTime:int;
      
      public var adBox:MovieClip;
      
      public var mcSTC:MovieClip;
      
      public var bPlay:SimpleButton;
      
      public var mcCover:MovieClip;
      
      public var mcSUSPEND:MovieClip;
      
      public var mcCMG:MovieClip;
      
      public var mcFlashVersionCover:MovieClip;
      
      public var startTime2:int;
      
      public function MainTimeline()
      {
         super();
         addFrameScript(0,frame1,10,frame11,20,frame21,50,frame51,60,frame61,70,frame71,80,frame81,91,frame92,96,frame97,97,frame98,98,frame99,99,frame100);
      }
      
      public function STATE_INITLOAD() : *
      {
         var e_GOTOBOXHEAD:Function = null;
         var e_GOTOCMG:Function = null;
         e_GOTOBOXHEAD = function(param1:MouseEvent):void
         {
            CGlobalData.OpenURL_BoxheadWebsite();
         };
         e_GOTOCMG = function(param1:MouseEvent):void
         {
            CGlobalData.OpenURL_SponsorWebsite();
            CMochiBot.Track(CMochiBot.BXH_ZW_SPONSORCLICK_LOADING);
         };
         if(!CheckVersion())
         {
            stop();
            return;
         }
         mcFlashVersionCover.visible = false;
         this.mcLoaderBar.visible = false;
         this.mcLoadingText.visible = false;
         this.mTimeStart = getTimer();
         this.mcLogo.addEventListener(MouseEvent.MOUSE_UP,e_GOTOBOXHEAD);
         this.bMoreGames.addEventListener(MouseEvent.MOUSE_UP,e_GOTOCMG);
         if(CGlobalData.isLIVE().mLive || CProtection.isCMG() || CProtection.isKONGREGATE() && CBuild.mKongregateStats)
         {
            gotoAndPlay("STARTLOAD");
         }
         else
         {
            gotoAndPlay("COUNTDOWN");
         }
      }
      
      internal function frame100() : *
      {
         mcFlashVersionCover.visible = false;
      }
      
      internal function frame71() : *
      {
         STATE_FADEOUT();
      }
      
      public function SendToURL(param1:String) : void
      {
         var loader:URLLoader = null;
         var request:URLRequest = null;
         var url:String = param1;
         loader = new URLLoader();
         loader.addEventListener(Event.COMPLETE,completeHandler);
         loader.addEventListener(Event.OPEN,openHandler);
         loader.addEventListener(ProgressEvent.PROGRESS,progressHandler);
         loader.addEventListener(SecurityErrorEvent.SECURITY_ERROR,securityErrorHandler);
         loader.addEventListener(HTTPStatusEvent.HTTP_STATUS,httpStatusHandler);
         loader.addEventListener(IOErrorEvent.IO_ERROR,ioErrorHandler);
         request = new URLRequest(url);
         try
         {
            loader.load(request);
         }
         catch(error:Error)
         {
         }
      }
      
      internal function frame81() : *
      {
         STATE_LOADED();
      }
      
      public function STATE_PLAYGAME() : *
      {
         this.bPlay.alpha = 0;
         this.addEventListener(Event.ENTER_FRAME,e_PLAYFADE);
         startTime = getTimer();
         stop();
      }
      
      public function e_COVERFADE(param1:Event) : void
      {
         var _loc2_:Number = NaN;
         _loc2_ = (getTimer() - startTime) / (1 * 1000);
         this.mcCover.alpha = _loc2_;
         if(this.mcCover.alpha >= 1)
         {
            this.removeEventListener(Event.ENTER_FRAME,e_COVERFADE);
            this.gotoAndStop("LOADED");
         }
      }
      
      internal function frame98() : *
      {
         stop();
         mcSTC.START();
      }
      
      public function httpStatusHandler(param1:HTTPStatusEvent) : void
      {
      }
      
      public function STATE_SUSPEND() : void
      {
         this.mcSUSPEND.visible = true;
         stop();
      }
      
      internal function frame97() : *
      {
         mcSTC.visible = false;
         mcCMG.visible = false;
         mcFlashVersionCover.visible = false;
         mcSTC.addEventListener("FINISHED",e_STCFINISHED);
         if(CBuild.mSkipIntro)
         {
            gotoAndStop(3);
         }
         else
         {
            nextFrame();
         }
      }
      
      public function STATE_FADEOUT() : *
      {
         this.mcCover.alpha = 0;
         this.addEventListener(Event.ENTER_FRAME,e_COVERFADE);
         startTime = getTimer();
      }
      
      public function progressHandler(param1:ProgressEvent) : void
      {
      }
      
      public function ioErrorHandler(param1:IOErrorEvent) : void
      {
      }
      
      public function e_COVERFADE_OUT(param1:Event) : void
      {
         var _loc2_:Number = NaN;
         _loc2_ = (getTimer() - startTime2) / (1 * 1000);
         mcCover.alpha = 1 - _loc2_;
         if(mcCover.alpha <= 0)
         {
            this.removeEventListener(Event.ENTER_FRAME,e_COVERFADE_OUT);
            gotoAndStop(1,"MAIN");
         }
      }
      
      internal function frame92() : *
      {
         stop();
      }
      
      public function e_PLAYFADE(param1:Event) : void
      {
         var _loc2_:Number = NaN;
         _loc2_ = (getTimer() - startTime) / (1 * 1000);
         this.bPlay.alpha = _loc2_;
         this.mcLoaderBar.alpha = this.mcLoadingText.alpha = 1 - _loc2_;
         if(this.bPlay.alpha >= 1)
         {
            this.removeEventListener(Event.ENTER_FRAME,e_PLAYFADE);
            this.bPlay.addEventListener(MouseEvent.MOUSE_UP,e_PLAYGAME);
         }
      }
      
      internal function frame21() : *
      {
         STATE_STARTLOAD();
      }
      
      internal function frame99() : *
      {
         this.addEventListener(Event.ENTER_FRAME,e_COVERFADE_OUT);
         mcCover.alpha = 1;
         startTime2 = getTimer();
         stop();
      }
      
      public function securityErrorHandler(param1:SecurityErrorEvent) : void
      {
      }
      
      internal function frame1() : *
      {
         currentPercentage = 0;
         this.stage.scaleMode = StageScaleMode.NO_SCALE;
         this.stage.align = StageAlign.TOP_LEFT;
         this.stage.showDefaultContextMenu = false;
         mcSUSPEND.visible = false;
         tfDebug.visible = false;
         CProtection.mRoot = root;
         CProtection.tfDebug = tfDebug;
         CMochiBot.mRoot = root;
         CGlobalData.SendData_Visit();
         CMochiBot.Track(CMochiBot.BXH_ZW_VISITS);
         tfVersion.visible = true;
         tfVersion.text = CVersion.mVersionString + " " + Capabilities.version + " (" + CProtection.GetDomain() + ")";
         CProtection.ShowDebugInfo();
         STATE_INITLOAD();
      }
      
      public function e_STCFINISHED(param1:Event) : void
      {
         mcSTC.visible = false;
         mcSTC.removeEventListener("FINISHED",e_STCFINISHED);
         mcCMG.visible = true;
         mcCMG.addEventListener("FINISHED",e_CMGFINISHED);
         mcCMG.START();
      }
      
      public function openHandler(param1:Event) : void
      {
      }
      
      internal function frame11() : *
      {
         STATE_SUSPEND();
      }
      
      public function e_COUNTDOWNFADE(param1:Event) : void
      {
         var _loc2_:Number = NaN;
         var _loc3_:Object = null;
         _loc2_ = (getTimer() - startTime) / (1 * 1000);
         this.mcCountDown.alpha = Math.min(1,_loc2_);
         _loc3_ = CGlobalData.isLIVE();
         this.mcCountDown.tfTime.text = _loc3_.mDays + " days " + CString.Pad(_loc3_.mHours,"0",2) + ":" + CString.Pad(_loc3_.mMinutes,"0",2) + ":" + CString.Pad(_loc3_.mSeconds,"0",2);
         if(_loc3_.mLive)
         {
            this.removeEventListener(Event.ENTER_FRAME,e_COUNTDOWNFADE);
            gotoAndPlay(1);
         }
      }
      
      public function STATE_LOADED() : *
      {
         this.gotoAndStop(1,"INTRO");
      }
      
      public function STATE_COUNTDOWN() : void
      {
         var e_GOTOCMG:Function = null;
         e_GOTOCMG = function(param1:MouseEvent):void
         {
            CProtection.NavigateToURL(CGlobalData.mCMGBoxheadTheZombieWarsURL);
            CMochiBot.Track(CMochiBot.BXH_ZW_SPONSORCLICK_LOADING);
            CMochiBot.Track(CMochiBot.BXH_ZW_TOTALSPONSORCLICKS);
         };
         this.mcLoaderBar.visible = false;
         stop();
         this.mcCountDown.alpha = 0;
         this.addEventListener(Event.ENTER_FRAME,e_COUNTDOWNFADE);
         this.mcCountDown.bHere.addEventListener(MouseEvent.MOUSE_UP,e_GOTOCMG);
         startTime = getTimer();
      }
      
      internal function frame51() : *
      {
         STATE_COUNTDOWN();
      }
      
      public function e_PLAYGAME(param1:MouseEvent) : void
      {
         this.gotoAndStop("FADEOUT");
      }
      
      public function CheckVersion() : Boolean
      {
         var _loc1_:String = null;
         var _loc2_:Array = null;
         var _loc3_:Array = null;
         var _loc4_:Object = null;
         var _loc5_:Object = null;
         _loc1_ = Capabilities.version;
         _loc2_ = _loc1_.split(" ");
         _loc3_ = _loc2_[1].split(",");
         _loc4_ = {
            "majorVersion":9,
            "buildNumber":47
         };
         _loc5_ = {
            "majorVersion":Number(_loc3_[0]),
            "buildNumber":Number(_loc3_[2])
         };
         mcFlashVersionCover.tfTitles.text = "current version:\nrequired version:";
         mcFlashVersionCover.tfValues.text = _loc2_[1] + "\n" + _loc4_.majorVersion + ",0," + _loc4_.buildNumber + ",0";
         if(_loc5_.majorVersion < _loc4_.majorVersion || _loc5_.majorVersion == _loc4_.majorVersion && _loc5_.buildNumber < _loc4_.buildNumber)
         {
            return false;
         }
         return true;
      }
      
      public function completeHandler(param1:Event) : void
      {
         var _loc2_:URLLoader = null;
         var _loc3_:String = null;
         _loc2_ = URLLoader(param1.target);
         _loc3_ = _GetField(_loc2_.data,"src");
      }
      
      public function _GetField(param1:String, param2:String) : String
      {
         var _loc3_:int = 0;
         var _loc4_:int = 0;
         param2 += "=" + String.fromCharCode(34);
         _loc3_ = int(param1.indexOf(param2));
         if(_loc3_ == -1)
         {
            return "";
         }
         param1 = param1.substr(_loc3_ + param2.length);
         _loc4_ = int(param1.indexOf(String.fromCharCode(34)));
         if(_loc4_ == -1)
         {
            return "";
         }
         return param1.substring(0,_loc4_);
      }
      
      public function e_CMGFINISHED(param1:Event) : void
      {
         mcCMG.removeEventListener("FINISHED",e_CMGFINISHED);
         nextFrame();
      }
      
      public function STATE_STARTLOAD() : *
      {
         this.addEventListener(Event.ENTER_FRAME,PL_LOADING);
         this.mcLoaderBar.visible = true;
         this.mcLoadingText.visible = true;
         currentPercentage = 0;
         if(CProtection.mSHOWAD)
         {
            AdLoader.LoadAd(adBox);
         }
         stop();
      }
      
      public function PL_LOADING(param1:Event) : void
      {
         var _loc2_:Number = NaN;
         var _loc3_:Number = NaN;
         _loc2_ = this.loaderInfo.bytesLoaded / this.loaderInfo.bytesTotal * 100;
         _loc3_ = (getTimer() - this.mTimeStart) / (CBuild.mLoadingTime * 1000) * 100;
         _loc2_ = _loc3_ < _loc2_ ? _loc3_ : _loc2_;
         if(int(_loc2_) > currentPercentage && currentPercentage < 100)
         {
            ++currentPercentage;
         }
         this.mcLoaderBar.gotoAndStop(int(_loc2_ + 1));
         if(currentPercentage >= 100)
         {
            this.removeEventListener(Event.ENTER_FRAME,PL_LOADING);
            this.gotoAndStop("PLAYGAME");
         }
      }
      
      internal function frame61() : *
      {
         STATE_PLAYGAME();
      }
   }
}

