package
{
   import Landscape.CLSCtrlCoverMask;
   
   [Embed(source="/_assets/assets.swf", symbol="symbol717")]
   public dynamic class Brush_Cover extends CLSCtrlCoverMask
   {
      
      public function Brush_Cover()
      {
         super();
         addFrameScript(0,frame1);
      }
      
      internal function frame1() : *
      {
         visible = false;
      }
   }
}

