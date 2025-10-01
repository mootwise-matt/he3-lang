# Event System & Concurrency

## Overview

He³ provides a VM-managed event system that enables secure, type-safe communication across threads. Events allow modules to communicate without direct dependencies, promoting loose coupling and scalable architecture.

## Event System API

### Core Interfaces
```he3
interface Event<T> {
  function publish(data: T): void;
  function subscribe(callback: function(T): void): EventSubscription;
  function unsubscribe(subscription: EventSubscription): void;
  function getSubscriberCount(): integer;
}

interface EventSubscription {
  var id: string;
  var callback: function(T): void;
  var isActive: boolean;
}
```

### Event Manager
```he3
class EventManager {
  static function createEvent<T>(name: string): Event<T>;
  static function getEvent<T>(name: string): Option<Event<T>>;
  static function listEvents(): Array<string>;
  static function removeEvent(name: string): boolean;
}
```

## Event Declaration and Usage

### Event Types
```he3
// Define event data structures
record UserLoginEvent {
  var userId: string;
  var timestamp: integer;
  var ipAddress: string;
  var userAgent: string;
}

record PaymentProcessedEvent {
  var orderId: string;
  var amount: float;
  var status: PaymentStatus;
  var transactionId: string;
}

record SystemShutdownEvent {
  var reason: string;
  var gracefulShutdown: boolean;
  var estimatedTime: integer;
}
```

### Event Creation
```he3
domain app.events {
  // Create global events
  let userLoginEvent = EventManager.createEvent<UserLoginEvent>("user.login");
  let paymentEvent = EventManager.createEvent<PaymentProcessedEvent>("payment.processed");
  let systemShutdownEvent = EventManager.createEvent<SystemShutdownEvent>("system.shutdown");
}
```

### Event Publishing
```he3
domain app.auth {
  class AuthenticationService {
    function loginUser(credentials: LoginCredentials): Result<User, AuthError> {
      // ... authentication logic ...
      
      if (user.isValid) {
        // Publish event across all threads
        userLoginEvent.publish(UserLoginEvent {
          userId: user.id,
          timestamp: System.currentTimeMillis(),
          ipAddress: request.ipAddress,
          userAgent: request.userAgent
        });
        
        return Ok(user);
      }
      
      return Err(AuthError.InvalidCredentials);
    }
  }
}
```

### Event Subscription
```he3
domain app.notifications {
  class NotificationService {
    var subscriptions: Array<EventSubscription>;
    
    function initialize() {
      // Subscribe to events from any thread
      this.subscriptions.append(
        userLoginEvent.subscribe(function(event: UserLoginEvent) {
          this.sendWelcomeEmail(event.userId);
          this.logUserActivity(event);
        })
      );
      
      this.subscriptions.append(
        paymentEvent.subscribe(function(event: PaymentProcessedEvent) {
          this.sendReceipt(event.orderId);
          this.updateUserPreferences(event.userId);
        })
      );
    }
    
    function cleanup() {
      // Unsubscribe from all events
      for (subscription in this.subscriptions) {
        userLoginEvent.unsubscribe(subscription);
        paymentEvent.unsubscribe(subscription);
      }
    }
  }
}
```

## Thread-Safe Event Handling

### Concurrent Event Processing
```he3
domain app.analytics {
  class AnalyticsCollector {
    var eventBuffer: Array<AnalyticsEvent>;
    var mutex: Mutex;
    var subscriptions: Array<EventSubscription>;
    
    function initialize() {
      this.mutex = new Mutex();
      this.eventBuffer = new Array<AnalyticsEvent>();
      
      // Subscribe to multiple events
      this.subscriptions.append(
        userLoginEvent.subscribe(function(event: UserLoginEvent) {
          this.recordEvent(AnalyticsEvent {
            type: "user_login",
            userId: event.userId,
            timestamp: event.timestamp,
            metadata: event.userAgent
          });
        })
      );
      
      this.subscriptions.append(
        paymentEvent.subscribe(function(event: PaymentProcessedEvent) {
          this.recordEvent(AnalyticsEvent {
            type: "payment",
            orderId: event.orderId,
            amount: event.amount,
            timestamp: System.currentTimeMillis()
          });
        })
      );
    }
    
    function recordEvent(event: AnalyticsEvent) {
      // Thread-safe event recording
      this.mutex.lock();
      this.eventBuffer.append(event);
      
      // Process buffer if it reaches threshold
      if (this.eventBuffer.length >= 100) {
        this.flushEvents();
      }
      
      this.mutex.unlock();
    }
    
    function flushEvents() {
      // Send events to analytics service
      for (event in this.eventBuffer) {
        this.sendToAnalyticsService(event);
      }
      this.eventBuffer.clear();
    }
  }
}
```

## Secure Events

### Security Integration
```he3
domain app.secure {
  class SecureEventManager {
    static function createSecureEvent<T>(
      name: string, 
      requiredKey: string
    ): SecureEvent<T>;
  }
  
  class SecureEvent<T> {
    var requiredKey: string;
    var event: Event<T>;
    var securityContext: SecurityContext;
    
    function publish(data: T, sessionToken: SessionToken): Result<void, SecurityError> {
      if (!this.validateSecurityToken(sessionToken)) {
        return Err(SecurityError.InvalidToken);
      }
      
      this.event.publish(data);
      return Ok(());
    }
    
    function subscribe(
      callback: function(T): void, 
      sessionToken: SessionToken
    ): Result<EventSubscription, SecurityError> {
      if (!this.validateSecurityToken(sessionToken)) {
        return Err(SecurityError.InvalidToken);
      }
      
      return Ok(this.event.subscribe(callback));
    }
    
    private function validateSecurityToken(token: SessionToken): boolean {
      return this.securityContext.validateToken(token, this.requiredKey);
    }
  }
}
```

## Event Patterns

### Request-Response Pattern
```he3
record DataRequestEvent {
  var requestId: string;
  var dataType: string;
  var parameters: Map<string, string>;
}

record DataResponseEvent {
  var requestId: string;
  var data: Option<Array<byte>>;
  var error: Option<string>;
}

// Request side
function requestData(dataType: string, params: Map<string, string>) {
  let requestId = System.generateUUID();
  let requestEvent = EventManager.getEvent<DataRequestEvent>("data.request");
  
  requestEvent.publish(DataRequestEvent {
    requestId: requestId,
    dataType: dataType,
    parameters: params
  });
  
  // Subscribe to response
  let responseEvent = EventManager.getEvent<DataResponseEvent>("data.response");
  responseEvent.subscribe(function(response: DataResponseEvent) {
    if (response.requestId == requestId) {
      this.handleDataResponse(response);
    }
  });
}
```

### Event Aggregation
```he3
class EventAggregator {
  var eventCounts: Map<string, integer>;
  var lastEventTimes: Map<string, integer>;
  
  function initialize() {
    // Subscribe to all system events
    for (eventName in EventManager.listEvents()) {
      this.trackEvent(eventName);
    }
  }
  
  private function trackEvent(eventName: string) {
    let event = EventManager.getEvent<SystemEvent>(eventName);
    event.subscribe(function(eventData: SystemEvent) {
      this.eventCounts.increment(eventName);
      this.lastEventTimes.put(eventName, System.currentTimeMillis());
    });
  }
}
```

## VM Event Manager Implementation

### Event Context Structure
```c
typedef struct {
    char* eventName;
    void* eventData;
    size_t dataSize;
    Array* subscribers;
    pthread_mutex_t* mutex;
    bool isSecure;
    char* requiredKey;
} EventContext;

typedef struct {
    char* subscriptionId;
    CallbackFunction callback;
    pthread_t threadId;
    bool isActive;
} EventSubscriber;
```

### Thread-Safe Operations
```c
// Thread-safe event publishing
void publish_event(VM* vm, char* eventName, void* data, size_t dataSize) {
    EventContext* ctx = find_event_context(vm, eventName);
    if (!ctx) return;
    
    pthread_mutex_lock(ctx->mutex);
    
    // Notify all subscribers across threads
    for (int i = 0; i < ctx->subscribers->length; i++) {
        EventSubscriber* sub = ctx->subscribers->data[i];
        if (sub->isActive) {
            // Queue callback for execution in subscriber's thread
            queue_callback(sub->threadId, sub->callback, data, dataSize);
        }
    }
    
    pthread_mutex_unlock(ctx->mutex);
}

// Cross-thread callback execution
void execute_callback(VM* vm, CallbackFunction callback, void* data) {
    // Execute callback in target thread's context
    push_to_thread_stack(vm, callback, data);
    signal_thread(vm, callback->threadId);
}
```

## Best Practices

1. **Event Naming**: Use hierarchical naming (e.g., "user.login", "payment.processed")
2. **Type Safety**: Always use strongly-typed event data structures
3. **Memory Management**: Unsubscribe from events when no longer needed
4. **Error Handling**: Handle event processing errors gracefully
5. **Performance**: Use event batching for high-frequency events
6. **Security**: Use secure events for sensitive data
7. **Testing**: Mock events for unit testing
8. **Documentation**: Document event contracts and data structures
