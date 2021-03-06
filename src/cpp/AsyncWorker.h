
#include <pifacedigital.h>


struct ASyncInfo
{
// required
	uv_work_t request; // libuv
	Persistent<Function> callback; // javascript callback
// optional : data goes here.
// data that doesn't go back to javascript can be any typedef
// data that goes back to javascript needs to be a supported type
  int hw_addr;
  int timeout;
  uint8_t inputs;
  int success;
};


// called by libuv worker in separate thread
static void WaitForAsync(uv_work_t *req)
{
	ASyncInfo *info = static_cast<ASyncInfo *>(req->data);
	info->success = pifacedigital_wait_for_input(&inputs, info->timeout, info->hw_addr);
	if (info->success==0){
		printf("Interrupt detected. Inputs: 0x%x\n", info->inputs);	
	}else {
		printf("pifacedigital_wait_for_input error or timeout\n");
	}
	
}



static void AsyncAfter(uv_work_t *req, int status)
{
	// get the reference to the baton from the request
	ASyncInfo *info = static_cast<ASyncInfo *>(req->data);
	// set up return arguments
	Handle<Value> argv[] =
	{
		Handle<Value>(Int32::New(info->success)),
		Handle<Value>(String::New(info->input))
	};
	// execute the callback
	info->callback->Call(Context::GetCurrent()->Global(),2,argv);
	// dispose the callback object from the info
	info->callback.Dispose();
	// delete the baton object
	delete info;
}


/*
class AsyncWorker : public NanAsyncWorker {
 public:
  AsyncWorker(NanCallback callback, int hw_addr): NanAsyncWorker(callback), points(points) {}
  ~AsyncWorker() {}

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on this.
  void Execute () {
    pifacedigital_enable_interrupts();
    int success = pifacedigital_wait_for_input(&this->inputs, -1, this->hw_addr); ) > 0){
    printf("Interrupt detected. Inputs: 0x%x\n", this->inputs);
    pifacedigital_disable_interrupts();
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback () {
    NanScope();
    printf("HandleOKCallback\n");
    Local<Value> argv[] = { Local<Value>::New(Null()) , Number::New(inputs) };
    callback->Call(2, argv);
  };

 private:
  int hw_addr;
  uint8_t inputs;
};
*/