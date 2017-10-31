/*
 * elevator sstf
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

struct sstf_data {
	struct list_head queue;
};

static void sstf_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

static int sstf_dispatch(struct request_queue *q, int force)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (!list_empty(&nd->queue)) {
		struct request *rq;//located in blkdev, this is the item we are sorting
		rq = list_entry(nd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);//this function deletes entry from list and reinitializes it.
		elv_dispatch_sort(q, rq);
		return 1;
	}
	return 0;
}

static void sstf_add_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	struct list_head *iterator = NULL; //set up an iterator
	struct list_head *printer = NULL;

	int i=0;

	if(!list_empty(&nd->queue)){
		struct request *previous = list_entry(nd->queue.prev, struct request, queuelist);
		/*list_for_each(iterator, &nd->queue){
			struct request *temp = list_entry(iterator, struct request, queuelist);
			previous = list_entry(previous->queuelist.prev, struct request, queuelist);
			i = i + 1;
			if(blk_rq_pos(rq)<blk_rq_pos(temp)){//We want the list to be ordered
		   		list_add(&rq->queuelist, &previous->queuelist);//add the element in the location immediately  before the first value it is larger than
		   		printk("ADDING REQUEST TO POSITION %d IN QUEUE\n", i);
		   		break;
			}
			if(temp->queuelist.next == &nd->queue){
			   	list_add_tail(&rq->queuelist, &nd->queue);	
				printk("ADDED REQUEST TO END OF QUEUE");
				
			}
		}*/ /*THIS WAS A FIRST ATTEMPT AT THE ALGO IT CAN BE SIMPLIFIED GREATLY WITH THE TWO LINES BELOW"*/
		while(blk_rq_pos(rq) < blk_rq_pos(previous)){
		   	previous = list_entry(previous->queuelist.prev, struct request, queuelist);
		}
		list_add(&rq->queuelist, &previous->queuelist);
			
	}
	if(list_empty(&nd->queue)){
		printk("EMPTY LIST... ADDING TO TAIL AS DEFAULT\n");
		list_add(&rq->queuelist, &nd->queue);
	}

	printk("REQUEST QUEUE: ");
	list_for_each(printer,&nd->queue){
	   struct request *printed=list_entry(printer, struct request, queuelist);
	   printk(" %llu\n", blk_rq_pos(printed));
	}
}

static struct request *
sstf_former_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
sstf_latter_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static int sstf_init_queue(struct request_queue *q, struct elevator_type *e)
{
	struct sstf_data *nd;
	struct elevator_queue *eq;

	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	eq->elevator_data = nd;

	INIT_LIST_HEAD(&nd->queue);

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);
	return 0;
}

static void sstf_exit_queue(struct elevator_queue *e)
{
	struct sstf_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_sstf = {
	.ops = {
		.elevator_merge_req_fn		= sstf_merged_requests,
		.elevator_dispatch_fn		= sstf_dispatch,
		.elevator_add_req_fn		= sstf_add_request,
		.elevator_former_req_fn		= sstf_former_request,
		.elevator_latter_req_fn		= sstf_latter_request,
		.elevator_init_fn		= sstf_init_queue,
		.elevator_exit_fn		= sstf_exit_queue,
	},
	.elevator_name = "sstf",
	.elevator_owner = THIS_MODULE,
};

static int __init sstf_init(void)
{
	return elv_register(&elevator_sstf);
}

static void __exit sstf_exit(void)
{
	elv_unregister(&elevator_sstf);
}

module_init(sstf_init);
module_exit(sstf_exit);


MODULE_AUTHOR("Jens Axboe");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("No-op IO scheduler");
