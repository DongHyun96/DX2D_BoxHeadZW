package Game
{
   import IO.*;
   import STC9.IO.*;
   import STC9.String.*;
   import STC9.System.Profiler.*;
   import com.kongregate.as3.client.KongregateAPI;
   import flash.display.*;
   import flash.events.*;
   import flash.external.*;
   import flash.geom.*;
   import flash.media.*;
   import flash.net.*;
   import flash.system.*;
   import flash.text.*;
   import flash.utils.getTimer;
   
   public class CGlobalData
   {
      
      private static var mCurrentSendScore:Boolean;
      
      private static var mSharedObject:SharedObject;
      
      private static var mCurrentLevelIndex:int;
      
      public static var mKongregate:KongregateAPI;
      
      public static var mStats:Object;
      
      private static var mData:Object;
      
      private static var mCurrentDifficulyLevel:int;
      
      public static var mLocalStats:Object;
      
      private static var mKongStatFieldUpdate:Object;
      
      private static const key:String = "J54D15DIB50AJBB2EAEGE55GECC0GG1F0DCJBH4E2GE400GBI143AI4G0IIHFGEA";
      
      private static const lKey:int = 63;
      
      public static const mGamesURL:String = "http://www.games.seantcooper.com/";
      
      public static const mBoxheadURL:String = "http://www.boxhead.seantcooper.com/";
      
      public static const mProjectURL:String = mBoxheadURL + "Games/BXH_ZW/";
      
      public static const mInstructionsURL:String = mProjectURL + "GameGuide.aspx";
      
      public static const mTrackingURL:String = mProjectURL + "Tracker.aspx";
      
      public static const mCMGURL:String = "http://www.crazymonkeygames.com/";
      
      public static const mCMGScoreURL:String = "http://scores.crazymonkeygames.com/hs/listscores.php?id=";
      
      public static const mMoreGamesURL:String = mCMGURL;
      
      public static const mCMGBoxheadHalloweenURL:String = mCMGURL + "Boxhead.html";
      
      public static const mCMGBoxheadTheRoomsURL:String = mCMGURL + "Boxhead-The-Rooms.html";
      
      public static const mCMGBoxheadMoreRoomsURL:String = mCMGURL + "Boxhead-More-Rooms.html";
      
      public static const mCMGBoxhead2PlayRoomsURL:String = mCMGURL + "Boxhead-2Play-Rooms.html";
      
      public static const mCMGBoxheadTheZombieWarsURL:String = mCMGURL + "Boxhead-The-Zombie-Wars.html";
      
      public static const mMochiBotURL:String = "http://www.mochibot.com/";
      
      public static const mFGLURL:String = "http://www.flashgamelicense.com/game_referral.php?name=Boxheads-The%20Zombie%20Wars";
      
      public static const mWWLiveDate:Date = new Date(2008,3 - 1,7 + 7);
      
      public function CGlobalData()
      {
         super();
      }
      
      public static function GetData(param1:String, param2:* = null) : *
      {
         OpenSharedObject();
         if(mData[param1] == null)
         {
            SetData(param1,param2);
            return param2;
         }
         return mData[param1];
      }
      
      public static function LoadKongregate(param1:DisplayObjectContainer) : void
      {
         if(CBuild.mKongregateStats && (CProtection.isKONGREGATE() || CBuild.mDEBUG))
         {
            if(!mKongregate)
            {
               mKongregate = new KongregateAPI();
               mKongregate.visible = false;
            }
            if(!param1.contains(mKongregate))
            {
               param1.addChild(mKongregate);
            }
         }
      }
      
      public static function AddStat(param1:String, param2:*, param3:String = "HIGHEST") : void
      {
         var _loc4_:String = null;
         OpenSharedObject();
         _AddStat(mStats,param1,param2,param3);
         if(_AddStat(mLocalStats,param1,param2,param3))
         {
            mKongStatFieldUpdate[param1] = mLocalStats[param1];
         }
         if(getTimer() - mKongStatFieldUpdate["__timer"] > 5000)
         {
            for(_loc4_ in mKongStatFieldUpdate)
            {
               KongregateSubmitStat(_loc4_,"",mKongStatFieldUpdate[_loc4_]);
            }
            mKongStatFieldUpdate = new Object();
            mKongStatFieldUpdate["__timer"] = getTimer();
         }
      }
      
      private static function KongregateSubmitStat(param1:String, param2:String, param3:*) : void
      {
         var _loc4_:String = null;
         if(!mCurrentSendScore)
         {
            return;
         }
         if(!mKongregate)
         {
            return;
         }
         _loc4_ = mCurrentLevelIndex >= 4 ? "M1" : "M0";
         param1 = param1.toLowerCase();
         switch(param1)
         {
            case "zombie kills":
            case "devil kills":
            case "mummy kills":
            case "vampire kills":
            case "runner kills":
               mKongregate.stats.submit(param2 + param1,param3);
               break;
            case "highest score":
            case "highest level":
            case "highest score multiplier":
               mKongregate.stats.submit(param2 + _loc4_ + "D" + mCurrentDifficulyLevel + " " + param1,param3);
         }
      }
      
      public static function SetData(param1:String, param2:*) : void
      {
         OpenSharedObject();
         mData[param1] = param2;
      }
      
      private static function OpenSharedObject() : void
      {
         if(mSharedObject)
         {
            return;
         }
         mSharedObject = SharedObject.getLocal("BXH_ZOMBIEWARS_7892334");
         if(mSharedObject.data.mStats == null)
         {
            mSharedObject.data.mStats = new Object();
         }
         if(mSharedObject.data.mData == null)
         {
            mSharedObject.data.mData = new Object();
         }
         mStats = mSharedObject.data.mStats;
         mData = mSharedObject.data.mData;
         Dump();
      }
      
      public static function OpenURL_GamesWebsite() : void
      {
         CProtection.NavigateToURL(CGlobalData.mGamesURL);
         CMochiBot.Track(CMochiBot.BXH_ZW_GAMESWEBSITEVISITS);
      }
      
      public static function isLIVE() : Object
      {
         var _loc1_:Date = null;
         var _loc2_:Number = NaN;
         var _loc3_:Object = null;
         _loc1_ = new Date();
         _loc2_ = mWWLiveDate.time - _loc1_.time;
         _loc3_ = new Object();
         _loc3_.mMilliseconds = Math.floor(_loc2_ % 1000);
         _loc3_.mSeconds = Math.floor(_loc2_ / 1000 % 60);
         _loc3_.mMinutes = Math.floor(_loc2_ / (1000 * 60) % 60);
         _loc3_.mHours = Math.floor(_loc2_ / (1000 * 60 * 60) % 24);
         _loc3_.mDays = Math.floor(_loc2_ / (1000 * 60 * 60 * 24));
         _loc3_.mLive = CProtection.isLICENSEE() ? true : (_loc2_ >= 0 ? false : true);
         return _loc3_;
      }
      
      public static function OpenURL_SponsorWebsite() : void
      {
         OpenSponsorURL(CGlobalData.mCMGURL);
      }
      
      public static function SendData_Visit() : void
      {
      }
      
      private static function Dump() : void
      {
         if(mSharedObject == null)
         {
            return;
         }
      }
      
      public static function PostKongregateStats() : void
      {
         var _loc1_:String = null;
         for(_loc1_ in mLocalStats)
         {
            KongregateSubmitStat(_loc1_,"",mLocalStats[_loc1_]);
         }
      }
      
      public static function SendData_LevelPlay(param1:int) : void
      {
      }
      
      private static function _AddStat(param1:Object, param2:String, param3:*, param4:String = "HIGHEST") : Boolean
      {
         if(param1[param2] == null)
         {
            param1[param2] = param3;
            return true;
         }
         switch(param4)
         {
            case "HIGHEST":
               if(param3 > param1[param2])
               {
                  param1[param2] = param3;
                  return true;
               }
               break;
            case "ADD":
               if(param3 != 0)
               {
                  param1[param2] += param3;
                  return true;
               }
               break;
            case "SET":
               param1[param2] = param3;
               return true;
         }
         return false;
      }
      
      private static function Encode(param1:String) : String
      {
         var _loc2_:String = null;
         var _loc3_:int = 0;
         var _loc4_:int = 0;
         var _loc5_:int = 0;
         var _loc6_:int = 0;
         var _loc7_:* = 0;
         _loc2_ = "";
         _loc3_ = Math.random() * 9999;
         _loc4_ = 0;
         while(_loc4_ < param1.length)
         {
            _loc5_ = int(param1.charCodeAt(_loc4_));
            _loc6_ = int(key.charCodeAt(_loc4_ & lKey));
            _loc7_ = _loc5_ + _loc6_ + _loc3_ & 0xFF;
            _loc2_ += CString.Pad(_loc7_.toString(16),"0",2);
            _loc4_++;
         }
         return _loc2_.toUpperCase() + CString.Pad(String(_loc3_),"0",4);
      }
      
      public static function StartLocalStats(param1:int, param2:int, param3:Boolean) : void
      {
         mLocalStats = new Object();
         mKongStatFieldUpdate = new Object();
         mCurrentLevelIndex = param1;
         mCurrentDifficulyLevel = param2;
         mCurrentSendScore = param3;
         mKongStatFieldUpdate["__timer"] = getTimer();
      }
      
      public static function OpenURL_BoxheadWebsite() : void
      {
         CProtection.NavigateToURL(CGlobalData.mBoxheadURL);
         CMochiBot.Track(CMochiBot.BXH_ZW_BOXHEADWEBSITEVISITS);
      }
      
      public static function Flush() : void
      {
         if(mSharedObject)
         {
            mSharedObject.flush();
            Dump();
         }
      }
      
      private static function SendData(param1:String, param2:URLVariables = null) : void
      {
         var request:URLRequest = null;
         var nVariables:URLVariables = null;
         var trackingType:String = param1;
         var variables:URLVariables = param2;
         if(CBuild.mDEBUG)
         {
            return;
         }
         variables = variables == null ? new URLVariables() : variables;
         variables.trackid = trackingType;
         variables.domain = new LocalConnection().domain.toLowerCase();
         request = new URLRequest(mTrackingURL);
         nVariables = new URLVariables();
         nVariables.session = Encode(variables.toString());
         request.data = nVariables;
         try
         {
            sendToURL(request);
         }
         catch(e:Error)
         {
         }
      }
      
      public static function OpenSponsorURL(param1:String) : void
      {
         CProtection.NavigateToURL(param1);
         CMochiBot.Track(CMochiBot.BXH_ZW_TOTALSPONSORCLICKS);
      }
      
      public function get _CLASSID_() : String
      {
         return "CGlobalData";
      }
   }
}

