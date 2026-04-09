package
{
   import Landscape.CLSCtrlCollision;
   
   [Embed(source="/_assets/assets.swf", symbol="symbol715")]
   public dynamic class Brush_Collision extends CLSCtrlCollision
   {
      
      public function Brush_Collision()
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

