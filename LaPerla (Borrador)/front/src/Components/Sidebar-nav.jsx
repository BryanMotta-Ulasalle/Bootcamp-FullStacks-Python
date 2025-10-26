export function SidebarNav({ icon, label, active, onClick }) {
  return (
    <button
      onClick={onClick}
      className={`flex items-center gap-5 p-3 w-full justify-start transition-all duration-200
        ${active
          ? "bg-primary text-black"
          : "hover:bg-primary/20 text-black/60 dark:text-white/60 hover:text-black dark:hover:text-white"}
      `}
    >
      {icon}
      <span
        className="whitespace-nowrap opacity-0 translate-x-[-10px] 
                   group-hover:opacity-100 group-hover:translate-x-0 
                   transition-all duration-200  text-xl"
      >
        {label}
      </span>
    </button>
  );
}
